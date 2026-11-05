# c-assembler

Fork of [MarcoMeijer/c-assembler](https://github.com/MarcoMeijer/c-assembler) with extra functionality.

## Building and running

```
gcc -Wall -o assembler main.c scanner.c parser.c
./assembler                     # compiles example.asm
./assembler program.asm         # compiles another file
./assembler -o out.txt program.asm   # writes the binary to a file
./assembler -h                  # help
```

On Windows (MinGW) the executable is `assembler.exe`.

## Instruction set

```
mov reg1, CONSTANT ; Move CONSTANT into register 1
    ; 00 CCCCCC where C is the bits of the constant

mov A, B ; Move register B into register A
    ; 10 BBB AAA (source in the middle bits, destination in the low bits,
    ; as in the original implementation)

or A, reg1, reg2 ; Perform reg1 | reg2 and store result in register A
    ; 01 AAA 000

add A, reg1, reg2 ; Perform reg1 + reg2 and store result in register A
    ; 01 AAA 001

sub A, reg1, reg2 ; Perform reg1 - reg2 and store result in register A
    ; 01 AAA 010

nand A, reg1, reg2 ; Perform ~(reg1 & reg2) and store result in register A
    ; 01 AAA 011

and A, reg1, reg2 ; Perform reg1 & reg2 and store result in register A
    ; 01 AAA 100

not A, reg1 ; Perform ~reg1 and store result in register A
    ; 01 AAA 101

xor A, reg1, reg2 ; Perform reg1 ^ reg2 and store result in register A
    ; 01 AAA 110

eq A, reg1, reg2 ; Perform reg1 == reg2 and store result in register A
    ; 01 AAA 111

mov reg1, [reg2] ; Read from RAM at address register 2 and store result in register 1
    ; 11 000000

mov [reg2], reg1 ; Move register 1 into RAM at address register 2
    ; 11 000001
```

## Added functionality

| # | Feature | Points |
|---|---------|--------|
| 1 | Comments: everything after `;` until the end of the line is ignored | 50 |
| 2 | `not A, reg1` instruction | 50 |
| 3 | Command line flags: input file, `-o <file>`, `-h` / `--help` | 50 |
| 4 | `or`, `add`, `sub`, `nand`, `and`, `xor`, `eq` instructions | 100 |
| 5 | Writing to RAM: `mov [reg2], reg1` -> `11000001` | 50 |
| 6 | Reading from RAM: `mov reg1, [reg2]` -> `11000000` | 50 |
| 7 | Constants into any register: `mov reg2, 10` becomes `mov reg1, 10` + `mov reg2, reg1` | 50 |
| 8 | Error messages now show the token that was received | 50 |

Notes:

- Feature 7 uses reg1 as a temporary register, so `mov regX, CONSTANT` (X != 1) overwrites reg1.
- ALU instructions check that the operands are exactly `reg1` and `reg2` (in that order), because the hardware always reads those two registers.
- Examples of the new error messages:
  - `Compile error on line 1: unknown identifier 'foo'.`
  - `Error on line 1: Unexpected token 'reg1', expected an instruction.`
  - `Error on line 1: Expected either a register, a constant or [reg2] for move operation, but got ','.`

## Bug fixes

- `tokens` and `final_binary` were defined in the header files, which causes a
  "multiple definition" linker error with GCC 10 or newer. They are now declared
  `extern` in the headers and defined in the `.c` files.
- `read_file` did not add a `'\0'` at the end of the buffer, but the scanner
  relies on it to detect the end of the file (undefined behaviour). It also
  checked `buffer == NULL` instead of `*buffer == NULL`.
- Added bounds checks for the token array and the output binary.
- Very large constants can no longer overflow `int` and bypass the size check.
- Opening a file that does not exist now prints an error message.
