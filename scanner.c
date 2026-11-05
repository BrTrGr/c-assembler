#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"

Token tokens[MAX_TOKENS];

void read_file(char *filename, char **buffer) {
  long length;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    printf("Error: could not open file '%s'.\n", filename);
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  // +1 so we can add a '\0' at the end: the scanner uses it to detect EOF.
  *buffer = malloc(length + 1);
  if (*buffer == NULL) {
    printf("Error: out of memory.\n");
    exit(2);
  }
  size_t read = fread(*buffer, 1, length, f);
  (*buffer)[read] = '\0';
  fclose(f);
}

struct Scanner {
  char *start;
  char *current;
  int line;
} scanner;

void init_scanner(char *file_name) {
  char *file_content;
  read_file(file_name, &file_content);
  scanner.start = file_content;
  scanner.current = file_content;
  scanner.line = 1;
}

char peek_char() {
  return *scanner.current;
}

char consume_char() {
  char c = *scanner.current;
  scanner.current += 1;
  return c;
}

int get_scanner_length() {
  return (int)(scanner.current - scanner.start);
}

Token create_token(TokenType type, int value) {
  Token token;
  token.type = type;
  token.value = value;
  token.source = scanner.start;
  token.line = scanner.line;
  token.length = get_scanner_length();
  return token;
}

void print_token_text(Token t) {
  if (t.type == TOKEN_EOF)
    printf("end of file");
  else
    printf("'%.*s'", t.length, t.source);
}

// Skips whitespace AND comments. A comment starts with ';' and runs until the
// end of the line. The '\n' itself is left for the loop so the line counter
// stays correct.
void consume_whitespace() {
  while (true) {
    char c = peek_char();
    if (c == ';') {
      while (peek_char() != '\n' && peek_char() != '\0')
        consume_char();
      continue;
    }
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
      return;
    if (c == '\n')
      scanner.line += 1;
    consume_char();
  }
}

bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_alpha_numeric(char c) {
  return is_alpha(c) || is_digit(c);
}

Token create_int(char c) {
  int result = c - '0';
  while (is_digit(peek_char())) {
    c = consume_char();
    // Stop growing once the number is clearly too big, so huge constants
    // can't overflow the int and sneak past the "> 63" check in the parser.
    if (result < 1000000)
      result = (result * 10) + (c - '0'); // append the digit c to result
  }
  return create_token(TOKEN_INTEGER, result);
}

bool is_token_equal_to(const char *s) {
  int scan_len = get_scanner_length();
  if (scan_len != (int)strlen(s))
    return false;
  for (int i=0; i<scan_len; i++)
    if (s[i] != scanner.start[i])
      return false;
  return true;
}

typedef struct {
  const char *name;
  TokenType type;
  int value;
} Keyword;

// ALU opcodes are the last 3 bits of the instruction: 01 AAA ooo
static const Keyword keywords[] = {
  {"mov",  TOKEN_MOV,      0},
  {"or",   TOKEN_ALU,      0}, // 000
  {"add",  TOKEN_ALU,      1}, // 001
  {"sub",  TOKEN_ALU,      2}, // 010
  {"nand", TOKEN_ALU,      3}, // 011
  {"and",  TOKEN_ALU,      4}, // 100
  {"not",  TOKEN_NOT,      5}, // 101
  {"xor",  TOKEN_ALU,      6}, // 110
  {"eq",   TOKEN_ALU,      7}, // 111
  {"reg1", TOKEN_REGISTER, 1},
  {"reg2", TOKEN_REGISTER, 2},
  {"reg3", TOKEN_REGISTER, 3},
  {"reg4", TOKEN_REGISTER, 4},
  {"reg5", TOKEN_REGISTER, 5},
  {"reg6", TOKEN_REGISTER, 6},
};

Token create_identifier() {
  while (is_alpha_numeric(peek_char())) {
    consume_char();
  }

  int count = sizeof(keywords) / sizeof(keywords[0]);
  for (int i = 0; i < count; i++) {
    if (is_token_equal_to(keywords[i].name))
      return create_token(keywords[i].type, keywords[i].value);
  }

  printf("Compile error on line %d: unknown identifier '%.*s'.\n",
         scanner.line, get_scanner_length(), scanner.start);
  exit(1);
}

Token scan_token() {
  consume_whitespace();

  scanner.start = scanner.current;
  char c = consume_char();

  if (c == ',')
    return create_token(TOKEN_COMMA, 0);
  if (c == '[')
    return create_token(TOKEN_LBRACKET, 0);
  if (c == ']')
    return create_token(TOKEN_RBRACKET, 0);
  if (is_digit(c))
    return create_int(c);
  if (is_alpha(c))
    return create_identifier();
  if (c == '\0')
    return create_token(TOKEN_EOF, 0);

  printf("Compile error on line %d: Unexpected character '%c'.\n", scanner.line, c);
  exit(1);
}

void scan_tokens() {
  for (int i=0; true; i++) {
    if (i >= MAX_TOKENS) {
      printf("Compile error: file has too many tokens (max %d).\n", MAX_TOKENS);
      exit(1);
    }
    tokens[i] = scan_token();
    if (tokens[i].type == TOKEN_EOF)
      return;
  }
}
