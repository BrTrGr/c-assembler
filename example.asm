; Original example
mov reg1, 10
mov reg5, reg1
mov reg2, reg3

; Constants into any register (expands to 2 instructions)
mov reg2, 5     ; -> mov reg1, 5 + mov reg2, reg1

; ALU instructions: result register, reg1, reg2
or   reg3, reg1, reg2
add  reg3, reg1, reg2
sub  reg3, reg1, reg2
nand reg3, reg1, reg2
and  reg3, reg1, reg2
not  reg3, reg1
xor  reg3, reg1, reg2
eq   reg3, reg1, reg2

; RAM
mov [reg2], reg1  ; write reg1 to RAM[reg2]
mov reg1, [reg2]  ; read RAM[reg2] into reg1
