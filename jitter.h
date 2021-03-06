//
// Created by Michael Nolan on 10/6/16.
//

#ifndef D16_JIT_JITTER_H
#define D16_JIT_JITTER_H

#ifndef NO_JIT
#define JIT_ONLY(x) x
#else
#define JIT_ONLY(x)
#endif

#include <stdint.h>
typedef void (*jit_function)(void*);
jit_function branch_to(uint16_t addr, uint8_t rewrite);
void init_jit();


typedef struct{
    uint16_t regs[8];
    uint16_t link_register;
    uint16_t padding;
    uint32_t instructions_executed;
    uint8_t flag_z:1;
    uint8_t flag_c:1;
    uint8_t flag_n:1;
    uint8_t flag_v:1;
    
} processor_state;
#define INSTRS_EXECUTED 20 //index into struct
enum _Op_Type {
    NOP = 0,
    ADD,
    SUB,
    PUSH,
    POP,
    MOVB_R0,
    MOVB_R1,
    MOVB_R2,
    MOVB_R3,
    MOVB_R4,
    MOVB_R5,
    MOVB_R6,
    MOVB_R7,
    MOV = 0x0D,
    AND,
    OR,
    XOR,
    NOT,
    NEG,
    LD,
    ST,
    CMP,
    JMP,
    CALL,
    SPEC,
    SHL,
    SHR,
    ROL,
    RCL,
    LDCP,
    STCP,
    ADC,
    SBB,
    SET,
    TEST,
    PUSHLR,
    SAR,

    ADDI = 0x81,
    SUBI = 0x82,
    PUSHI = 0x83,
    MOVI = 0x8D,
    ANDI = 0x8e,
    ORI = 0x8f,
    XORI = 0x90,
    LDI = 0x93,
    STI = 0x94,
    CMPI = 0x95,
    JMPI = 0x96,
    CALLI = 0x97,
    SHLI = 0x99,
    SHRI = 0x9a,
    ROLI = 0x9b,
    RCLI = 0x9c,
    ADCI = 0x9f,
    SBBI = 0xA0,
    TESTI = 0xA2,
    SARI = 0xA4,
    KILL = 0xFF
};
typedef enum _condition_code {
    NV = 0,
    EQ = 1,
    NE,
    OS,
    OC,
    HI,
    LS,
    P,
    N,
    CS,
    CC,
    GE,
    G,
    LE,
    L,
    AL = 15
} condition_code;
#define BYTE_FLAG 0x80
#define DISP_FLAG 0x40
#endif //D16_JIT_JITTER_H
