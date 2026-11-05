#pragma once

#include <stdio.h>

typedef unsigned char u8;

#define MAX_BINARY_SIZE 256

// Defined in parser.c (see the note in scanner.h about "extern").
extern u8 final_binary[MAX_BINARY_SIZE];

void parse_lines();

// Prints the compiled program, one byte per line, to the given stream
// (stdout or a file).
void print_binary(FILE *out);
