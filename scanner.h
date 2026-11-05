#pragma once

typedef enum {
  TOKEN_REGISTER,   // reg1 .. reg6, value = register number
  TOKEN_INTEGER,    // decimal constant, value = the number
  TOKEN_MOV,        // mov
  TOKEN_ALU,        // or/add/sub/nand/and/xor/eq, value = 3-bit ALU opcode
  TOKEN_NOT,        // not, value = 3-bit ALU opcode (0b101)
  TOKEN_COMMA,      // ,
  TOKEN_LBRACKET,   // [
  TOKEN_RBRACKET,   // ]
  TOKEN_EOF,
} TokenType;

typedef struct {
  TokenType type;
  int value;
  char *source;
  int length;
  int line;
} Token;

#define MAX_TOKENS 10000

// Defined in scanner.c. Declaring it "extern" here (instead of defining it)
// avoids "multiple definition" linker errors on modern compilers.
extern Token tokens[MAX_TOKENS];

void init_scanner(char *file_name);

void scan_tokens();

// Prints the text of a token, e.g. 'reg3' or "end of file".
void print_token_text(Token t);
