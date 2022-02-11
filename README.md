# rv_sim
RISC-V Simulator

RV32I:
```
xxxxxxxxxxxxxxxxxxxxxxxxx0110111    U lui
xxxxxxxxxxxxxxxxxxxxxxxxx0010111    U auipc
xxxxxxxxxxxxxxxxxxxxxxxxx1101111    J jal
xxxxxxxxxxxxxxxxx000xxxxx1100111    I jalr
xxxxxxxxxxxxxxxxx000xxxxx1100011    B beq
xxxxxxxxxxxxxxxxx001xxxxx1100011    B bne
xxxxxxxxxxxxxxxxx100xxxxx1100011    B blt
xxxxxxxxxxxxxxxxx101xxxxx1100011    B bge
xxxxxxxxxxxxxxxxx110xxxxx1100011    B bltu
xxxxxxxxxxxxxxxxx111xxxxx1100011    B bgeu
xxxxxxxxxxxxxxxxx000xxxxx0000011    I lb
xxxxxxxxxxxxxxxxx001xxxxx0000011    I lh
xxxxxxxxxxxxxxxxx010xxxxx0000011    I lw
xxxxxxxxxxxxxxxxx100xxxxx0000011    I lbu
xxxxxxxxxxxxxxxxx101xxxxx0000011    I lhu
xxxxxxxxxxxxxxxxx000xxxxx0100011    S sb
xxxxxxxxxxxxxxxxx001xxxxx0100011    S sh
xxxxxxxxxxxxxxxxx010xxxxx0100011    S sw
xxxxxxxxxxxxxxxxx000xxxxx0010011    I addi
xxxxxxxxxxxxxxxxx010xxxxx0010011    I slti
xxxxxxxxxxxxxxxxx011xxxxx0010011    I sltiu
xxxxxxxxxxxxxxxxx100xxxxx0010011    I xori
xxxxxxxxxxxxxxxxx110xxxxx0010011    I ori
xxxxxxxxxxxxxxxxx111xxxxx0010011    I andi
0000000xxxxxxxxxx001xxxxx0010011    I slli
0000000xxxxxxxxxx101xxxxx0010011    I srli
0100000xxxxxxxxxx101xxxxx0010011    I srai
0000000xxxxxxxxxx000xxxxx0110011    R add
0100000xxxxxxxxxx000xxxxx0110011    R sub
0000000xxxxxxxxxx001xxxxx0110011    R sll
0000000xxxxxxxxxx010xxxxx0110011    R slt
0000000xxxxxxxxxx011xxxxx0110011    R sltu
0000000xxxxxxxxxx100xxxxx0110011    R xor
0000000xxxxxxxxxx101xxxxx0110011    R srl
0100000xxxxxxxxxx101xxxxx0110011    R sra
0000000xxxxxxxxxx110xxxxx0110011    R or
0000000xxxxxxxxxx111xxxxx0110011    R and
0000xxxxxxxx00000000000000001111    I fence
00000000000000000001000000001111    I fence.i
00000000000000000000000001110011    I ecall
00000000000100000000000001110011    I ebreak
xxxxxxxxxxxxxxxxx001xxxxx1110011    I csrrw
xxxxxxxxxxxxxxxxx010xxxxx1110011    I csrrs
xxxxxxxxxxxxxxxxx011xxxxx1110011    I csrrc
xxxxxxxxxxxxxxxxx101xxxxx1110011    I csrrwi
xxxxxxxxxxxxxxxxx110xxxxx1110011    I csrrsi
xxxxxxxxxxxxxxxxx111xxxxx1110011    I csrrci
```

Multiply/Divide:
```
0000001xxxxxxxxxx000xxxxx0110011    R mul
0000001xxxxxxxxxx001xxxxx0110011    R mulh
0000001xxxxxxxxxx010xxxxx0110011    R mulhsu
0000001xxxxxxxxxx011xxxxx0110011    R mulhu
0000001xxxxxxxxxx100xxxxx0110011    R div
0000001xxxxxxxxxx101xxxxx0110011    R divu
0000001xxxxxxxxxx110xxxxx0110011    R rem
0000001xxxxxxxxxx111xxxxx0110011    R remu
```

Floating point:
```
xxxxxxxxxxxxxxxxx010xxxxx0000111    I flw
xxxxxxxxxxxxxxxxx010xxxxx0100111    S fsw
xxxxx00xxxxxxxxxxxxxxxxxx1000011    R4 fmadd.s
xxxxx00xxxxxxxxxxxxxxxxxx1000111    R4 fmsub.s
xxxxx00xxxxxxxxxxxxxxxxxx1001011    R4 fnmsub.s
xxxxx00xxxxxxxxxxxxxxxxxx1001111    R4 fnmadd.s
0000000xxxxxxxxxxxxxxxxxx1010011    R fadd.s
0000100xxxxxxxxxxxxxxxxxx1010011    R fsub.s
0001000xxxxxxxxxxxxxxxxxx1010011    R fmul.s
0001100xxxxxxxxxxxxxxxxxx1010011    R fdiv.s
010110000000xxxxxxxxxxxxx1010011    R fsqrt.s
0010000xxxxxxxxxx000xxxxx1010011    R fsgnj.s
0010000xxxxxxxxxx001xxxxx1010011    R fsgnjn.s
0010000xxxxxxxxxx010xxxxx1010011    R fsgnjx.s
0010100xxxxxxxxxx000xxxxx1010011    R fmin.s
0010100xxxxxxxxxx001xxxxx1010011    R fmax.s
110000000000xxxxxxxxxxxxx1010011    R fcvt.w.s
110000000001xxxxxxxxxxxxx1010011    R fcvt.wu.s
111000000000xxxxx000xxxxx1010011    R fmv.x.w
1010000xxxxxxxxxx010xxxxx1010011    R feq.s
1010000xxxxxxxxxx001xxxxx1010011    R flt.s
1010000xxxxxxxxxx000xxxxx1010011    R fle.s
111000000000xxxxx001xxxxx1010011    R fclass.s
110100000000xxxxxxxxxxxxx1010011    R fcvt.s.w
110100000001xxxxxxxxxxxxx1010011    R fcvt.s.wu
111100000000xxxxx000xxxxx1010011    R fmv.w.x
xxxxxxxxxxxxxxxxx011xxxxx0000111    I fld
xxxxxxxxxxxxxxxxx011xxxxx0100111    S fsd
xxxxx01xxxxxxxxxxxxxxxxxx1000011    R4 fmadd.d
xxxxx01xxxxxxxxxxxxxxxxxx1000111    R4 fmsub.d
xxxxx01xxxxxxxxxxxxxxxxxx1001011    R4 fnmsub.d
xxxxx01xxxxxxxxxxxxxxxxxx1001111    R4 fnmadd.d
0000001xxxxxxxxxxxxxxxxxx1010011    R fadd.d
0000101xxxxxxxxxxxxxxxxxx1010011    R fsub.d
0001001xxxxxxxxxxxxxxxxxx1010011    R fmul.d
0001101xxxxxxxxxxxxxxxxxx1010011    R fdiv.d
010110100000xxxxxxxxxxxxx1010011    R fsqrt.d
0010001xxxxxxxxxx000xxxxx1010011    R fsgnj.d
0010001xxxxxxxxxx001xxxxx1010011    R fsgnjn.d
0010001xxxxxxxxxx010xxxxx1010011    R fsgnjx.d
0010101xxxxxxxxxx000xxxxx1010011    R fmin.d
0010101xxxxxxxxxx001xxxxx1010011    R fmax.d
010000000001xxxxxxxxxxxxx1010011    R fcvt.s.d
010000100000xxxxxxxxxxxxx1010011    R fcvt.d.s
1010001xxxxxxxxxx010xxxxx1010011    R feq.d
1010001xxxxxxxxxx001xxxxx1010011    R flt.d
1010001xxxxxxxxxx000xxxxx1010011    R fle.d
111000100000xxxxx001xxxxx1010011    R fclass.d
110000100000xxxxxxxxxxxxx1010011    R fcvt.w.d
110000100001xxxxxxxxxxxxx1010011    R fcvt.wu.d
110100100000xxxxxxxxxxxxx1010011    R fcvt.d.w
110100100001xxxxxxxxxxxxx1010011    R fcvt.d.wu
```

Atomic:
```
00010xx00000xxxxx010xxxxx0101111    R lr.w
00011xxxxxxxxxxxx010xxxxx0101111    R sc.w
00001xxxxxxxxxxxx010xxxxx0101111    R amoswap.w
00000xxxxxxxxxxxx010xxxxx0101111    R amoadd.w
00100xxxxxxxxxxxx010xxxxx0101111    R amoxor.w
01100xxxxxxxxxxxx010xxxxx0101111    R amoand.w
01000xxxxxxxxxxxx010xxxxx0101111    R amoor.w
10000xxxxxxxxxxxx010xxxxx0101111    R amomin.w
10100xxxxxxxxxxxx010xxxxx0101111    R amomax.w
11000xxxxxxxxxxxx010xxxxx0101111    R amominu.w
11100xxxxxxxxxxxx010xxxxx0101111    R amomaxu.w
```

Compressed:
```
000x00000xxxxx01    CI c.nop
000xxxxxxxxxxx01    CI c.addi
001xxxxxxxxxxx01    CJ c.jal
010xxxxxxxxxxx01    CI c.li
011x00010xxxxx01    CI c.addi16sp
011xxxxxxxxxxx01    CI c.lui
100x00xxxxxxxx01    CI c.srli
100x01xxxxxxxx01    CI c.srai
100x10xxxxxxxx01    CI c.andi
100011xxx00xxx01    CR c.sub
100011xxx01xxx01    CR c.xor
100011xxx10xxx01    CR c.or
100011xxx11xxx01    CR c.and
101xxxxxxxxxxx01    CJ c.j
110xxxxxxxxxxx01    CB c.beqz
111xxxxxxxxxxx01    CB c.bnez
0000000000000000    CIW Illegal instruction
000xxxxxxxxxxx00    CIW c.addi4spn
001xxxxxxxxxxx00    CL c.fld
010xxxxxxxxxxx00    CL c.lw
011xxxxxxxxxxx00    CL c.flw
101xxxxxxxxxxx00    CL c.fsd
110xxxxxxxxxxx00    CL c.sw
111xxxxxxxxxxx00    CL c.fsw
000xxxxxxxxxxx10    CI c.slli
0000xxxxx0000010    CI c.slli64
001xxxxxxxxxxx10    CSS c.fldsp
010xxxxxxxxxxx10    CSS c.lwsp
011xxxxxxxxxxx10    CSS c.flwsp
1000xxxxx0000010    CJ c.jr
1000xxxxxxxxxx10    CR c.mv
1001000000000010    CI c.ebreak
1001xxxxx0000010    CJ c.jalr
1001xxxxxxxxxx10    CR c.add
101xxxxxxxxxxx10    CSS c.fsdsp
110xxxxxxxxxxx10    CSS c.swsp
111xxxxxxxxxxx10    CSS c.fswsp
```

Privileged
```
00010000001000000000000001110011    R sret
00110000001000000000000001110011    R mret
00010000010100000000000001110011    R wfi
0001001xxxxxxxxxx000000001110011    R sfence.vma
```
