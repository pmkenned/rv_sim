#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#define NELEM(X) sizeof(X)/sizeof(X[0])

#define MASK_LSB_W(X, LSB, W) (X >> LSB) & ((1 << W) - 1)
#define MASK_MSB_LSB(X, MSB, LSB) (X >> LSB) & ((1 << (MSB-LSB+1)) - 1)

enum { MAX_STEPS = 100 };

enum {
    R_RA = 1,
    R_SP,
    R_GP,
    R_TP,
    R_T0,
    R_T1,
    R_T2,
    R_S0,
    R_FP = 8,
    R_S1,
    R_A0,
    R_A1,
    R_A2,
    R_A3,
    R_A4,
    R_A5,
    R_A6,
    R_A7,
    R_S2,
    R_S3,
    R_S4,
    R_S5,
    R_S6,
    R_S7,
    R_S8,
    R_S9,
    R_S10,
    R_S11,
    R_T3,
    R_T4,
    R_T5,
    R_T6,
};

typedef enum {
    FMT_R,
    FMT_I,
    FMT_S,
    FMT_B,
    FMT_U,
    FMT_J
} fmt_t;

typedef enum {
    MNEM_LUI,
    MNEM_AUIPC,
    MNEM_JAL,
    MNEM_JALR,
    MNEM_BEQ,
    MNEM_BNE,
    MNEM_BLT,
    MNEM_BGE,
    MNEM_BLTU,
    MNEM_BGEU,
    MNEM_LB,
    MNEM_LH,
    MNEM_LW,
    MNEM_LBU,
    MNEM_LHU,
    MNEM_SB,
    MNEM_SH,
    MNEM_SW,
    MNEM_ADDI,
    MNEM_SLTI,
    MNEM_SLTIU,
    MNEM_XORI,
    MNEM_ORI,
    MNEM_ANDI,
    MNEM_SLLI,
    MNEM_SRLI,
    MNEM_SRAI,
    MNEM_ADD,
    MNEM_SUB,
    MNEM_SLL,
    MNEM_SLT,
    MNEM_SLTU,
    MNEM_XOR,
    MNEM_SRL,
    MNEM_SRA,
    MNEM_OR,
    MNEM_AND,
    MNEM_FENCE,
    MNEM_FENCE_I,
    MNEM_ECALL,
    MNEM_EBREAK,
    MNEM_CSRRW,
    MNEM_CSRRS,
    MNEM_CSRRC,
    MNEM_CSRRWI,
    MNEM_CSRRSI,
    MNEM_CSRRCI
} mnemonic_t;

const char * op_mnemonics[] = {
    "lui",
    "auipc",
    "jal",
    "jalr",
    "beq",
    "bne",
    "blt",
    "bge",
    "bltu",
    "bgeu",
    "lb",
    "lh",
    "lw",
    "lbu",
    "lhu",
    "sb",
    "sh",
    "sw",
    "addi",
    "slti",
    "sltiu",
    "xori",
    "ori",
    "andi",
    "slli",
    "srli",
    "srai",
    "add",
    "sub",
    "sll",
    "slt",
    "sltu",
    "xor",
    "srl",
    "sra",
    "or",
    "and",
    "fence",
    "fence.i",
    "ecall",
    "ebreak",
    "csrrw",
    "csrrs",
    "csrrc",
    "csrrwi",
    "csrrsi",
    "csrrci"
};

const char * reg_names[] = {
    "x0",
    "ra",
    "sp",
    "gp",
    "tp",
    "t0",
    "t1",
    "t2",
    "fp",
    "s1",
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "a6",
    "a7",
    "s2",
    "s3",
    "s4",
    "s5",
    "s6",
    "s7",
    "s8",
    "s9",
    "s10",
    "s11",
    "t3",
    "t4",
    "t5",
    "t6"
};

typedef struct {
    fmt_t format;
    mnemonic_t mnemonic;
} instr_header_t;

typedef struct {
    instr_header_t header;
    int rd;
    int func3;
    int rs1;
    int rs2;
    int func7;
} r_instr_t;

typedef struct {
    instr_header_t header;
    int rd;
    int func3;
    int rs1;
    int imm_11_0;
} i_instr_t;

typedef struct {
    instr_header_t header;
    int imm_4_0;
    int func3;
    int rs1;
    int rs2;
    int imm_11_5;
} s_instr_t;

typedef struct {
    instr_header_t header;
    int imm_11_a7;
    int imm_4_1;
    int func3;
    int rs1;
    int rs2;
    int imm_12;
    int imm_10_5;
} b_instr_t;

typedef struct {
    instr_header_t header;
    int rd;
    int imm_31_12;
} u_instr_t;

typedef struct {
    instr_header_t header;
    int rd;
    int imm_19_12;
    int imm_11_a20;
    int imm_10_1;
    int imm_20;
} j_instr_t;

typedef union {
    r_instr_t ri;
    i_instr_t ii;
    s_instr_t si;
    b_instr_t bi;
    u_instr_t ui;
    j_instr_t ji;
} instr_t;

struct {
    uint32_t regs[32];
    uint32_t pc;
} state;

/* memory model */
/* TODO: use dictionary to implement sparse array */

uint32_t memory[256];

uint32_t
M_r(uint32_t addr, int size)
{
    uint32_t data;
    addr %= 1024;
    if (size == 1) {
        uint32_t x = memory[addr/4];
        if      (addr % 4 == 0) { data = (x & 0xff); }
        else if (addr % 4 == 1) { data = (x & 0xff00) >> 8; }
        else if (addr % 4 == 2) { data = (x & 0xff0000) >> 16; }
        else if (addr % 4 == 3) { data = (x & 0xff000000) >> 24; }
    } else if (size == 2) {
        uint32_t x = memory[addr/4];
        if      (addr % 2 == 0) { data = (x & 0xffff); }
        else if (addr % 2 == 1) { data = (x & 0xffff0000) >> 16; }
    } else if (size == 4) {
        data = memory[addr/4];
    }
    return data;
}

void
M_w(uint32_t addr, uint32_t data, int size)
{
    addr %= 1024;
    if (size == 1) {
        uint32_t x = memory[addr/4];
        if      (addr % 4 == 0) { x = (x & 0xffffff00) | (data & 0xff); }
        else if (addr % 4 == 1) { x = (x & 0xffff00ff) | ((data & 0xff) >> 8); }
        else if (addr % 4 == 2) { x = (x & 0xff00ffff) | ((data & 0xff) >> 16); }
        else if (addr % 4 == 3) { x = (x & 0x00ffffff) | ((data & 0xff) >> 24); }
        memory[addr/4] = x;
    } else if (size == 2) {
        uint32_t x = memory[addr/4];
        if      (addr % 2 == 0) { x = (x & 0xffff0000) | (data & 0xffff); }
        else if (addr % 2 == 1) { x = (x & 0x0000ffff) | ((data & 0xffff) >> 16); }
        memory[addr/4] = x;
    } else if (size == 4) {
        memory[addr/4] = data;
    }
}

/* TODO: handle RV64 */
static uint32_t
sext(uint32_t x, int w)
{
    if (w == 32)
        return x;
    int msb = (x >> (w-1)) & 0x1;
    uint32_t all1 = ~0;
    return msb ? x | ((all1 >> w) << w) : x & ((1 << w) - 1);
}

static uint32_t
get_imm(instr_t * i_p)
{
    uint32_t imm;
    instr_header_t * header_p = (instr_header_t *) i_p;
    switch (header_p->format) {
        case FMT_I:
            imm = sext(i_p->ii.imm_11_0, 12);
            break;
        case FMT_S:
            imm = sext((i_p->si.imm_11_5 << 5) + i_p->si.imm_4_0, 12);
            break;
        case FMT_B:
            imm = sext((i_p->bi.imm_12 << 12) + (i_p->bi.imm_11_a7 << 11) + (i_p->bi.imm_10_5 << 5) + (i_p->bi.imm_4_1 << 1), 13);
            break;
        case FMT_U:
            imm = (i_p->ui.imm_31_12 << 12);
            break;
        case FMT_J:
            imm = sext((i_p->ji.imm_20 << 20) + (i_p->ji.imm_19_12 << 12) + (i_p->ji.imm_11_a20 << 11) + (i_p->ji.imm_10_1 << 1), 21);
            break;
        default:
            imm = 0;
            assert(0);
            break;
    }
    return imm;
}

/* TODO: confirm that all instructions print correctly */
static int
snprint_instr(char * str, size_t size, instr_t * i_p)
{
    instr_header_t * header_p = (instr_header_t *) i_p;
    const char * mnemonic = op_mnemonics[header_p->mnemonic];

    const char * rd;
    const char * rs1;
    const char * rs2;
    int imm;
    int n;

    switch (header_p->format) {
        case FMT_R:
            rd  = reg_names[i_p->ri.rd];
            rs1 = reg_names[i_p->ri.rs1];
            rs2 = reg_names[i_p->ri.rs2];
            n = snprintf(str, size, "%s %s,%s,%s", mnemonic, rd, rs1, rs2);
            break;
        case FMT_I:
            rd  = reg_names[i_p->ii.rd];
            rs1 = reg_names[i_p->ii.rs1];
            imm = get_imm(i_p);
            if (mnemonic[0] == 'l') {
                n = snprintf(str, size, "%s %s,%d(%s)", mnemonic, rd, imm, rs1);
            } else if (mnemonic[0] == 'e') {
                n = snprintf(str, size, "%s", mnemonic);
            } else {
                n = snprintf(str, size, "%s %s,%s,%d", mnemonic, rd, rs1, imm);
            }
            break;
        case FMT_S:
            rs1 = reg_names[i_p->si.rs1];
            rs2 = reg_names[i_p->si.rs2];
            imm = get_imm(i_p);
            n = snprintf(str, size, "%s %s,%d(%s)", mnemonic, rs2, imm, rs1);
            break;
        case FMT_B:
            rs1 = reg_names[i_p->bi.rs1];
            rs2 = reg_names[i_p->bi.rs2];
            imm = get_imm(i_p);
            n = snprintf(str, size, "%s %s,%s,%d", mnemonic, rs1, rs2, imm);
            break;
        case FMT_U:
            rd = reg_names[i_p->ui.rd];
            imm = get_imm(i_p);
            n = snprintf(str, size, "%s %s,%d", mnemonic, rd, imm);
            break;
        case FMT_J:
            rd = reg_names[i_p->ji.rd];
            imm = get_imm(i_p);
            n = snprintf(str, size, "%s %s,%d", mnemonic, rd, imm);
            break;
        default:
            assert(0);
            break;
    }
    return n;
}

static void
print_instr(instr_t * i_p)
{
    static char buffer[1024];
    snprint_instr(buffer, sizeof(buffer), i_p);
    printf("%s\n", buffer);
}

/* TODO: some immediate field constraints are not included */
static instr_t
decode(uint32_t op)
{
    instr_t output;

    instr_header_t * header_p = (instr_header_t *) &output;

    int opcode      = MASK_MSB_LSB(op, 6, 0);
    int rd          = MASK_MSB_LSB(op, 11, 7);
    int rs1         = MASK_MSB_LSB(op, 19, 15);
    int rs2         = MASK_MSB_LSB(op, 24, 20);
    int func3       = MASK_MSB_LSB(op, 14, 12);
    int func7       = MASK_MSB_LSB(op, 31, 25);
    int imm_11_0    = MASK_MSB_LSB(op, 31, 20);
    int imm_4_0     = MASK_MSB_LSB(op, 11, 7);
    int imm_11_5    = MASK_MSB_LSB(op, 31, 25);
    int imm_11_a7   = MASK_MSB_LSB(op, 7, 7);
    int imm_4_1     = MASK_MSB_LSB(op, 11, 8);
    int imm_10_5    = MASK_MSB_LSB(op, 30, 25);
    int imm_12      = MASK_MSB_LSB(op, 31, 31);
    int imm_31_12   = MASK_MSB_LSB(op, 31, 12);
    int imm_19_12   = MASK_MSB_LSB(op, 19, 12);
    int imm_11_a20  = MASK_MSB_LSB(op, 20, 20);
    int imm_10_1    = MASK_MSB_LSB(op, 30, 21);
    int imm_20      = MASK_MSB_LSB(op, 31, 31);

    mnemonic_t m;

    switch (opcode) {
        case 0067: m = MNEM_LUI; break;
        case 0027: m = MNEM_AUIPC; break;
        case 0157: m = MNEM_JAL; break;
        case 0147:
            switch (func3) {
                case 0: m = MNEM_JALR; break;
                default: assert(0);
            }
            break;
        case 0143:
            switch (func3) {
                case 0: m = MNEM_BEQ; break;
                case 1: m = MNEM_BNE; break;
                case 4: m = MNEM_BLT; break;
                case 5: m = MNEM_BGE; break;
                case 6: m = MNEM_BLTU; break;
                case 7: m = MNEM_BGEU; break;
                default: assert(0);
            }
            break;
        case 0003:
            switch (func3) {
                case 0: m = MNEM_LB; break;
                case 1: m = MNEM_LH; break;
                case 2: m = MNEM_LW; break;
                case 4: m = MNEM_LBU; break;
                case 5: m = MNEM_LHU; break;
                default: assert(0);
            }
            break;
        case 0043:
            switch (func3) {
                case 0: m = MNEM_SB; break;
                case 1: m = MNEM_SH; break;
                case 2: m = MNEM_SW; break;
                default: assert(0);
            }
            break;
        case 0023:
            switch (func3) {
                case 0: m = MNEM_ADDI; break;
                case 2: m = MNEM_SLTI; break;
                case 3: m = MNEM_SLTIU; break;
                case 4: m = MNEM_XORI; break;
                case 6: m = MNEM_ORI; break;
                case 7: m = MNEM_ANDI; break;
                case 1:
                    switch (func7) {
                        case 0: m = MNEM_SLLI; break;
                        default: assert(0);
                    }
                    break;
                case 5:
                    switch (func7) {
                        case 0: m = MNEM_SRLI; break;
                        case 32: m = MNEM_SRAI; break;
                        default: assert(0);
                    }
                    break;
                default: assert(0);
            }
            break;
        case 0063:
            switch (func3) {
                case 0:
                    switch (func7) {
                        case 0: m = MNEM_ADD; break;
                        case 32: m = MNEM_SUB; break;
                        default: assert(0);
                    }
                    break;
                case 1: m = MNEM_SLL; break;
                case 2: m = MNEM_SLT; break;
                case 3: m = MNEM_SLTU; break;
                case 4: m = MNEM_XOR; break;
                case 5:
                    switch (func7) {
                        case 0: m = MNEM_SRL; break;
                        case 32: m = MNEM_SRA; break;
                        default: assert(0);
                    }
                    break;
                case 6: m = MNEM_OR; break;
                case 7: m = MNEM_AND; break;
                default: assert(0);
            }
            break;
        case 0017:
            switch (func3) {
                case 0: m = MNEM_FENCE; break;
                case 1: m = MNEM_FENCE_I; break;
                default: assert(0);
            }
            break;
        case 0163:
            switch (func3) {
                case 0:
                    switch (imm_11_0) {
                        case 0: m = MNEM_ECALL; break;
                        case 1: m = MNEM_EBREAK; break;
                        default: assert(0);
                    }
                    break;
                case 1: m = MNEM_CSRRW; break;
                case 2: m = MNEM_CSRRS; break;
                case 3: m = MNEM_CSRRC; break;
                case 5: m = MNEM_CSRRWI; break;
                case 6: m = MNEM_CSRRSI; break;
                case 7: m = MNEM_CSRRCI; break;
                default: assert(0);
            }
            break;
        default: assert(0);
    }

    fmt_t fmt;

    switch (m) {
        case MNEM_LUI:      fmt = FMT_U; break;
        case MNEM_AUIPC:    fmt = FMT_U; break;
        case MNEM_JAL:      fmt = FMT_J; break;
        case MNEM_JALR:     fmt = FMT_I; break;
        case MNEM_BEQ:      fmt = FMT_B; break;
        case MNEM_BNE:      fmt = FMT_B; break;
        case MNEM_BLT:      fmt = FMT_B; break;
        case MNEM_BGE:      fmt = FMT_B; break;
        case MNEM_BLTU:     fmt = FMT_B; break;
        case MNEM_BGEU:     fmt = FMT_B; break;
        case MNEM_LB:       fmt = FMT_I; break;
        case MNEM_LH:       fmt = FMT_I; break;
        case MNEM_LW:       fmt = FMT_I; break;
        case MNEM_LBU:      fmt = FMT_I; break;
        case MNEM_LHU:      fmt = FMT_I; break;
        case MNEM_SB:       fmt = FMT_S; break;
        case MNEM_SH:       fmt = FMT_S; break;
        case MNEM_SW:       fmt = FMT_S; break;
        case MNEM_ADDI:     fmt = FMT_I; break;
        case MNEM_SLTI:     fmt = FMT_I; break;
        case MNEM_SLTIU:    fmt = FMT_I; break;
        case MNEM_XORI:     fmt = FMT_I; break;
        case MNEM_ORI:      fmt = FMT_I; break;
        case MNEM_ANDI:     fmt = FMT_I; break;
        case MNEM_SLLI:     fmt = FMT_I; break;
        case MNEM_SRLI:     fmt = FMT_I; break;
        case MNEM_SRAI:     fmt = FMT_I; break;
        case MNEM_ADD:      fmt = FMT_R; break;
        case MNEM_SUB:      fmt = FMT_R; break;
        case MNEM_SLL:      fmt = FMT_R; break;
        case MNEM_SLT:      fmt = FMT_R; break;
        case MNEM_SLTU:     fmt = FMT_R; break;
        case MNEM_XOR:      fmt = FMT_R; break;
        case MNEM_SRL:      fmt = FMT_R; break;
        case MNEM_SRA:      fmt = FMT_R; break;
        case MNEM_OR:       fmt = FMT_R; break;
        case MNEM_AND:      fmt = FMT_R; break;
        case MNEM_FENCE:    fmt = FMT_I; break;
        case MNEM_FENCE_I:  fmt = FMT_I; break;
        case MNEM_ECALL:    fmt = FMT_I; break;
        case MNEM_EBREAK:   fmt = FMT_I; break;
        case MNEM_CSRRW:    fmt = FMT_I; break;
        case MNEM_CSRRS:    fmt = FMT_I; break;
        case MNEM_CSRRC:    fmt = FMT_I; break;
        case MNEM_CSRRWI:   fmt = FMT_I; break;
        case MNEM_CSRRSI:   fmt = FMT_I; break;
        case MNEM_CSRRCI:   fmt = FMT_I; break;
        default: assert(0); break;
    }

    header_p->mnemonic = m;
    header_p->format = fmt;

    switch (fmt) {
        case FMT_R:
            output.ri.rd = rd;
            output.ri.func3 = func3;
            output.ri.rs1 = rs1;
            output.ri.rs2 = rs2;
            output.ri.func7 = func7;
            break;
        case FMT_I:
            output.ii.rd = rd;
            output.ii.func3 = func3;
            output.ii.rs1 = rs1;
            output.ii.imm_11_0 = imm_11_0;
            break;

        case FMT_S:
            output.si.imm_4_0 = imm_4_0;
            output.si.func3 = func3;
            output.si.rs1 = rs1;
            output.si.rs2 = rs2;
            output.si.imm_11_5 = imm_11_5;
            break;

        case FMT_B:
            output.bi.imm_11_a7 = imm_11_a7;
            output.bi.imm_4_1 = imm_4_1;
            output.bi.func3 = func3;
            output.bi.rs1 = rs1;
            output.bi.rs2 = rs2;
            output.bi.imm_12 = imm_12;
            output.bi.imm_10_5 = imm_10_5;
            break;

        case FMT_U:
            output.ui.rd = rd;
            output.ui.imm_31_12 = imm_31_12;
            break;

        case FMT_J:
            output.ji.rd = rd;
            output.ji.imm_19_12 = imm_19_12;
            output.ji.imm_11_a20 = imm_11_a20;
            output.ji.imm_10_1 = imm_10_1;
            output.ji.imm_20 = imm_20;
            break;

        default:
            assert(0);
            break;

    }

    return output;
}

static void
reset()
{
    size_t i;
    for (i = 0; i < 32; i++) {
        state.regs[i] = 0;
    }
    state.pc = 0;

    /* TODO: remove these; put them in the test itself */
    state.regs[R_A1] = 5; // n 
    state.regs[R_A0] = 0x54; // &a[0]
    state.regs[R_RA] = 0x4c; // return address
}

/* TODO: pc, x[0] potentially bug-prone */
static int
step()
{
    uint32_t op = M_r(state.pc, 4);
    instr_t instr = decode(op);
    instr_header_t * header_p = (instr_header_t *) &instr;

    int32_t * x = (int32_t *) state.regs;
    uint32_t * x_u = state.regs;
    uint32_t pc = state.pc;
    uint32_t next_pc = pc + 4;

    int rd;
    int rs1;
    int rs2;
    int32_t imm;
    uint32_t imm_u;
    int32_t t;

    int shamt;
    int pred;
    int succ;
    int csr;
    int zimm;

    char buffer[1024];
    snprint_instr(buffer, sizeof(buffer), &instr);
    printf("%08x: %08x ; %s\n", (int) pc, op, buffer);

    switch (header_p->format) {
        case FMT_R:
            rd = instr.ri.rd;
            rs1 = instr.ri.rs1;
            rs2 = instr.ri.rs2;
            break;
        case FMT_I:
            rd = instr.ii.rd;
            rs1 = instr.ii.rs1;
            imm = get_imm(&instr);
            shamt = MASK_MSB_LSB(instr.ii.imm_11_0, 4, 0);
            pred  = MASK_MSB_LSB(instr.ii.imm_11_0, 7, 4);
            succ  = MASK_MSB_LSB(instr.ii.imm_11_0, 3, 0);
            csr   = instr.ii.imm_11_0;
            zimm  = instr.ii.rs1;
            break;
        case FMT_S:
            rs1 = instr.si.rs1;
            rs2 = instr.si.rs2;
            imm = get_imm(&instr);
            imm_u = (uint32_t) imm;
            break;
        case FMT_B:
            rs1 = instr.bi.rs1;
            rs2 = instr.bi.rs2;
            imm = get_imm(&instr);
            imm_u = (uint32_t) imm;
            break;
        case FMT_U:
            rd = instr.ii.rd;
            imm = get_imm(&instr);
            imm_u = (uint32_t) imm;
            break;
        case FMT_J:
            rd = instr.ii.rd;
            imm = get_imm(&instr);
            imm_u = (uint32_t) imm;
            break;
        default:
            assert(0);
            break;
    }

    switch (header_p->mnemonic) {
        case MNEM_LUI:
            x[rd] = imm;
            break;
        case MNEM_AUIPC:
            x[rd] = pc + imm;
            break;
        case MNEM_JAL:
            x[rd] = pc + 4;
            next_pc = pc + imm;
            break;
        case MNEM_JALR:
            t = pc + 4;
            next_pc = (x[rs1] + imm) & ~1;
            x[rd] = t;
            break;
        case MNEM_BEQ:
            if (x[rs1] == x[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_BNE:
            if (x[rs1] != x[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_BLT:
            if (x[rs1] < x[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_BGE:
            if (x[rs1] >= x[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_BLTU:
            if (x_u[rs1] < x_u[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_BGEU:
            if (x_u[rs1] >= x_u[rs2])
                next_pc = pc + imm;
            break;
        case MNEM_LB:
            x[rd] = sext(M_r(x[rs1] + imm, 1),8);
            break;
        case MNEM_LH:
            x[rd] = sext(M_r(x[rs1] + imm, 2),16);
            break;
        case MNEM_LW:
            x[rd] = sext(M_r(x[rs1] + imm, 4),32);
            break;
        case MNEM_LBU:
            x[rd] = M_r(x[rs1] + imm, 1);
            break;
        case MNEM_LHU:
            x[rd] = M_r(x[rs1] + imm, 2);
            break;
        case MNEM_SB:
            M_w(x[rs1] + imm, x[rs2], 1);
            break;
        case MNEM_SH:
            M_w(x[rs1] + imm, x[rs2], 2);
            break;
        case MNEM_SW:
            M_w(x[rs1] + imm, x[rs2], 4);
            break;
        case MNEM_ADDI:
            x[rd] = x[rs1] + imm;
            break;
        case MNEM_SLTI:
            x[rd] = x[rs1] < imm;
            break;
        case MNEM_SLTIU:
            x[rd] = x_u[rs1] < imm_u;
            break;
        case MNEM_XORI:
            x[rd] = x[rs1] ^ imm;
            break;
        case MNEM_ORI:
            x[rd] = x[rs1] | imm;
            break;
        case MNEM_ANDI:
            x[rd] = x[rs1] & imm;
            break;
        case MNEM_SLLI:
            x[rd] = x[rs1] << shamt;
            break;
        case MNEM_SRLI:
            /* TODO: add static assertion that right-shifting of signed value is arithmetic */
            x[rd] = x_u[rs1] >> shamt;
            break;
        case MNEM_SRAI:
            x[rd] = x[rs1] >> shamt;
            break;
        case MNEM_ADD:
            x[rd] = x[rs1] + x[rs2];
            break;
        case MNEM_SUB:
            x[rd] = x[rs1] - x[rs2];
            break;
        case MNEM_SLL:
            x[rd] = x[rs1] << x[rs2];
            break;
        case MNEM_SLT:
            x[rd] = x[rs1] < x[rs2];
            break;
        case MNEM_SLTU:
            x[rd] = x_u[rs1] < x_u[rs2];
            break;
        case MNEM_XOR:
            x[rd] = x[rs1] ^ x[rs1];
            break;
        case MNEM_SRL:
            x[rd] = x_u[rs1] >> x_u[rs2];
            break;
        case MNEM_SRA:
            x[rd] = x[rs1] >> x[rs2];
            break;
        case MNEM_OR:
            x[rd] = x[rs1] | x[rs1];
            break;
        case MNEM_AND:
            x[rd] = x[rs1] & x[rs1];
            break;
        case MNEM_FENCE:
            /* TODO */
            break;
        case MNEM_FENCE_I:
            /* TODO */
            break;
        case MNEM_ECALL:
            /* TODO */
            break;
        case MNEM_EBREAK:
            return 1;
            /* TODO */
            break;
        case MNEM_CSRRW:
            /* TODO */
            break;
        case MNEM_CSRRS:
            /* TODO */
            break;
        case MNEM_CSRRC:
            /* TODO */
            break;
        case MNEM_CSRRWI:
            /* TODO */
            break;
        case MNEM_CSRRSI:
            /* TODO */
            break;
        case MNEM_CSRRCI:
            /* TODO */
            break;
        default:
            assert(0);
            break;
    }

    state.pc = next_pc;
    x[0] = 0;

    return 0;
}

static void
print_state()
{
    size_t i;
    printf("\n");
    for (i = 0; i < 32; i++) {
        printf("x%zu: %x\n", i, state.regs[i]);
    }
    printf("pc: %x\n", state.pc);
    printf("memory:\n");
    for (i = 0; i < 1024/16; i++) {
        printf("M[%08x]: %08x %08x  %08x %08x\n", (int) i*16, M_r(i*16, 4), M_r(i*16+4, 4), M_r(i*16+8, 4), M_r(i*16+12, 4));
    }
    printf("\n");
}

int
main(int argc, char * argv[])
{
    size_t i;
    uint32_t * ops;
    size_t n_ops = 0;
    size_t ops_cap = 10;

    if (argc < 2) {
        fprintf(stderr, "usage: %s [FILE]\n", argv[0]);
        exit(1);
    }

    /* read hex file */
    FILE * fp = fopen(argv[1], "r");
    ops = malloc(sizeof(*ops) * ops_cap);
    n_ops = 0;
    while (1) {
        uint32_t x = 0;
        int rv = fscanf(fp, "%x", &x);
        if ((rv == 0) || (rv == EOF))
            break;
        ops[n_ops] = x;
        n_ops++;
        if (n_ops >= ops_cap) {
            ops_cap *= 2;
            ops = realloc(ops, sizeof(*ops) * ops_cap);
        }
    }
    fclose(fp);

    /* load program into memory */
    for (i = 0; i < n_ops; i++) {
        M_w(i*4, ops[i], 4);
    }

    reset();

    print_state();
    int step_cnt = 0;
    while (1) {
        if (step())
            break;
        if (step_cnt++ > MAX_STEPS) {
            fprintf(stderr, "exceeded MAX_STEPS, quitting\n");
            break;
        }
    }
    print_state();

    return 0;
}
