# TODO

- fence, fence.i, ecall, ebreak, csr
- RV64
- RV32M
- RV32F and RV32D
- RV32A
- RV32C
- RV32V
- Privileged arch
- Write assembler


## RISC-V Reader ERRATA:

pg 16, Figure 2.3
For lui and auipc, the immediate field is "imm[31:12]" when it should be "imm[19:0]"

pg 27, Figure 2.8
"a3 points to a[0]" should be "a0 points to a[0]"

pg 119, 11.9 "Concluding Remarks"
"If it succeeds, RISC-V will we be a significant break from the incremental ISAs of the past."

pg 126: beq, if (rs1 == rs2), this implies that if the *fields* are equal; it would be more consistent with the rest of the appendix if it said x[rs1] == x[rs2]

## Misc Notes

`_Static_assert (0, "assert1");`


```
        3322222          2222211111            111 11000           0000000
        1098765          4321098765            432 10987           6543210
R-type: func7..          rs2..rs1..            f3. rd...           opcode.
I-type: imm[11:0]        .....rs1..            f3. rd...           opcode.
S-type: imm[11:5]        rs2..rs1..            f3. imm[4:0]        opcode.
B-type: imm[12]imm[10:5] rs2..rs1..            f3. imm[4:1]imm[11] opcode.
U-type: imm[31:12]       ..........            ... rd...           opcode.
J-type: imm[20]imm[10:1] ....imm[11]imm[19:12] ... rd...           opcode.
```

Regs:
x0 / zero
x1 / ra
x2 / sp
x3 / gp
x4 / tp
x5 / t0
x6 / t1
x7 / t2
x8 / s0 / fp
x9 / s1
x10 / a0
x11 / a1
x12 / a2
x13 / a3
x14 / a4
x15 / a5
x16 / a6
x17 / a7
x18 / s2
x19 / s3
x20 / s4
x21 / s5
x22 / s6
x23 / s7
x24 / s8
x25 / s9
x26 / s10
x27 / s11
x28 / t3
x29 / t4
x30 / t5
x31 / t6
pc

### User-level CSRs

ustatus         0x000
fflags          0x001
frm             0x002
fcsr            0x003
uie             0x004
utvec           0x005
uscratch        0x040
uepc            0x041
ucause          0x042
utval           0x043
uip             0x044
cycle           0xc00
time            0xc01
instret         0xc02
hpmcounter3     0xc03
hpmcounter4     0xc04
...
hpmcounter31    0xc1f
cycleh          0xc80
timeh           0xc81
instreth        0xc82
hpmcounter3h    0xc83
hpmcounter4h    0xc84
...
hpmcounter31h   0xc9f

### Supervisor-level CSRs

sstatus
sedeleg
sideleg
sie
stvec
scounteren
sscratch
sepc
scause
stval
sip
satp

### Hypervisor-level CSRs

hstatus
hedeleg
hdie
hcounteren
hgeie
htval
hip
hvip
htinst
hgeip
hgatp
htimedelta
htimedeltah
vsstatus
vsie
vstvec
vsscratch
vsepc
vscause
vstval
vsip
vsatp

### Machine-level CSRs

mvendorid
marchid
mimpid
mhartid
mstatus
misa
medeleg
mideleg
mie
mtvec
mcounteren
mstatush
mscratch
mepc
mcause
mtval
mip
mtinst
mtval2
pmpcfg0
pmpcfg1
pmpcfg2
pmpcfg3
...
pmpcfg14
pmpcfg15
pmpaddr0
pmpaddr1
...
pmpaddr63
mcycle
minstret
mhpmcounter3
mhpmcounter4
...
mhpmcounter31
mcycleh
minstreth
mhpmcounter3h
mhpmcounter4h
...
mhpmcounter31h
mcouninhibit
mhpmevent3
mhpmevent4
...
mhpmevent31
tselect
tdata1
tdata2
tdata3
dscr
dpc
dscratch0
dscratch1

### Unknown

mtime
mtimecmp
