#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

u8 final_binary[MAX_BINARY_SIZE];

// Instruction prefixes (top 2 bits)
#define OP_CONSTANT 0x00 // 00 CCCCCC
#define OP_ALU      0x40 // 01 AAA ooo
#define OP_COPY     0x80 // 10 SSS DDD
#define OP_RAM      0xC0 // 11 ......

#define RAM_READ    0xC0 // mov reg1, [reg2] -> 11 000000
#define RAM_WRITE   0xC1 // mov [reg2], reg1 -> 11 000001

struct Parser {
  Token *current;
  int next_byte;
} parser;

Token peek() {
  return *parser.current;
}

Token consume() {
  Token t = *parser.current;
  if (t.type == TOKEN_EOF) {
    printf("Error on line %d: Unexpected end of file\n", t.line);
    exit(1);
  }
  parser.current++; // go to next token
  return t;
}

// Prints "Error on line X: <message>, but got 'token'." and stops.
void error_got(Token t, char *message) {
  printf("Error on line %d: %s, but got ", t.line, message);
  print_token_text(t);
  printf(".\n");
  exit(1);
}

Token expect(TokenType type, char *error_message) {
  Token t = *parser.current;
  if (t.type != type)
    error_got(t, error_message);
  return consume();
}

// Like expect(TOKEN_REGISTER), but also checks it is one specific register.
Token expect_register(int number, char *error_message) {
  Token t = expect(TOKEN_REGISTER, error_message);
  if (t.value != number)
    error_got(t, error_message);
  return t;
}

void init_parser() {
  parser.current = tokens;
  parser.next_byte = 0;
}

void compile_byte(u8 x) {
  if (parser.next_byte >= MAX_BINARY_SIZE) {
    printf("Error: program is too big (max %d bytes).\n", MAX_BINARY_SIZE);
    exit(1);
  }
  final_binary[parser.next_byte] = x;
  parser.next_byte += 1;
}

// Register-to-register copy: 10 SSS DDD (source in the middle bits,
// destination in the low bits, same as the original implementation).
void compile_copy(int source, int destination) {
  compile_byte(OP_COPY | (source << 3) | destination);
}

// mov [reg2], reg1   -> write register 1 into RAM at address reg2
void parse_ram_write() {
  // the '[' has already been consumed
  expect_register(2, "Only [reg2] can be used as a RAM address");
  expect(TOKEN_RBRACKET, "Expected ']'");
  expect(TOKEN_COMMA, "Expected comma");
  expect_register(1, "Only reg1 can be written to RAM");
  compile_byte(RAM_WRITE);
}

// mov reg1, [reg2]   -> read RAM at address reg2 into register 1
void parse_ram_read(Token destination) {
  // the '[' has already been consumed
  if (destination.value != 1) {
    printf("Error on line %d: Can only read RAM into reg1, but got ", destination.line);
    print_token_text(destination);
    printf(".\n");
    exit(1);
  }
  expect_register(2, "Only [reg2] can be used as a RAM address");
  expect(TOKEN_RBRACKET, "Expected ']'");
  compile_byte(RAM_READ);
}

void parse_move() {
  Token first = consume();

  if (first.type == TOKEN_LBRACKET) {
    parse_ram_write();
    return;
  }

  if (first.type != TOKEN_REGISTER)
    error_got(first, "You can only move into a register or into [reg2]");

  Token regis = first;
  expect(TOKEN_COMMA, "Expected comma");
  Token rhs = consume();

  // compile loading a constant into a register
  if (rhs.type == TOKEN_INTEGER) {
    if (rhs.value > 63) {
      printf("Error on line %d: Can't move numbers bigger than 63 into a register, but got ", rhs.line);
      print_token_text(rhs);
      printf(".\n");
      exit(1);
    }

    // The CPU can only load constants into register 1 ...
    compile_byte(OP_CONSTANT | rhs.value);

    // ... so for any other register we add a copy: mov regX, reg1
    // NOTE: this overwrites the previous value of reg1.
    if (regis.value != 1)
      compile_copy(1, regis.value);
    return;
  }

  // compile loading register into another register
  if (rhs.type == TOKEN_REGISTER) {
    compile_copy(rhs.value, regis.value);
    return;
  }

  // compile reading from RAM
  if (rhs.type == TOKEN_LBRACKET) {
    parse_ram_read(regis);
    return;
  }

  // invalid case
  error_got(rhs, "Expected either a register, a constant or [reg2] for move operation");
}

// or/add/sub/nand/and/xor/eq  A, reg1, reg2   -> 01 AAA ooo
// not                         A, reg1         -> 01 AAA 101
void parse_alu(Token operation) {
  Token destination = expect(TOKEN_REGISTER, "Expected a register to store the result in");
  expect(TOKEN_COMMA, "Expected comma");
  expect_register(1, "The first operand must be reg1");

  if (operation.type != TOKEN_NOT) {
    expect(TOKEN_COMMA, "Expected comma");
    expect_register(2, "The second operand must be reg2");
  }

  compile_byte(OP_ALU | (destination.value << 3) | operation.value);
}

void parse_line() {
  Token t = consume();

  if (t.type == TOKEN_MOV) {
    parse_move();
    return;
  }

  if (t.type == TOKEN_ALU || t.type == TOKEN_NOT) {
    parse_alu(t);
    return;
  }

  printf("Error on line %d: Unexpected token ", t.line);
  print_token_text(t);
  printf(", expected an instruction.\n");
  exit(1);
}

void parse_lines() {
  init_parser();

  while (peek().type != TOKEN_EOF) {
    parse_line();
  }
}

void print_binary(FILE *out) {
  for (int i=0; i<parser.next_byte; i++) {
    for (int j=7; j>=0; j--) {
      if (final_binary[i] & (1 << j)) {
        fprintf(out, "1");
      } else {
        fprintf(out, "0");
      }
    }
    fprintf(out, "\n");
  }
}
