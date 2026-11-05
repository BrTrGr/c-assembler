#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"

void print_usage(char *program_name) {
  printf("Usage: %s [options] [file]\n", program_name);
  printf("\n");
  printf("  file          assembly file to compile (default: example.asm)\n");
  printf("\n");
  printf("Options:\n");
  printf("  -o <file>     write the binary to <file> instead of the screen\n");
  printf("  -h, --help    show this help message\n");
}

int main(int argc, char *argv[]) {
  char *input_file = "example.asm";
  char *output_file = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) {
        printf("Error: -o needs a file name.\n");
        return 1;
      }
      output_file = argv[++i];
    } else if (argv[i][0] == '-') {
      printf("Error: unknown option '%s'.\n\n", argv[i]);
      print_usage(argv[0]);
      return 1;
    } else {
      input_file = argv[i];
    }
  }

  init_scanner(input_file);
  scan_tokens();
  parse_lines();

  if (output_file == NULL) {
    print_binary(stdout);
  } else {
    FILE *out = fopen(output_file, "w");
    if (out == NULL) {
      printf("Error: could not write to '%s'.\n", output_file);
      return 1;
    }
    print_binary(out);
    fclose(out);
    printf("Binary written to %s\n", output_file);
  }
  return 0;
}
