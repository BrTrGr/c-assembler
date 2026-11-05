# c-assembler

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

## Building and running

```
gcc -Wall -o assembler main.c scanner.c parser.c
./assembler                     # compiles example.asm
./assembler program.asm         # compiles another file
./assembler -o out.txt program.asm   # writes the binary to a file
./assembler -h                  # help
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
