//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#include "addr.h"

#define ADC_ASM 0x01
#define AND_ASM 0x02
#define ASL_ASM 0x03
#define BCC_ASM 0x04
#define BCS_ASM 0x05
#define BEQ_ASM 0x06
#define BIT_ASM 0x07
#define BMI_ASM 0x08
#define BNE_ASM 0x09
#define BPL_ASM 0x0a
#define BRK_ASM 0x0b
#define BVC_ASM 0x0c
#define BVS_ASM 0x0d
#define CLC_ASM 0x0e
#define CLD_ASM 0x0f
#define CLI_ASM 0x10
#define CLV_ASM 0x11
#define CMP_ASM 0x12
#define CPX_ASM 0x13
#define CPY_ASM 0x14
#define DEC_ASM 0x15
#define DEX_ASM 0x16
#define DEY_ASM 0x17
#define EOR_ASM 0x18
#define INC_ASM 0x19
#define INX_ASM 0x1a
#define INY_ASM 0x1b
#define JMP_ASM 0x1c
#define JSR_ASM 0x1d
#define LDA_ASM 0x1e
#define LDX_ASM 0x1f
#define LDY_ASM 0x20
#define LSR_ASM 0x21
#define NOP_ASM 0x22
#define ORA_ASM 0x23
#define PHA_ASM 0x24
#define PHP_ASM 0x25
#define PLA_ASM 0x26
#define PLP_ASM 0x27
#define ROL_ASM 0x28
#define ROR_ASM 0x29
#define RTI_ASM 0x2a
#define RTS_ASM 0x2b
#define SBC_ASM 0x2c
#define SEC_ASM 0x2d
#define SED_ASM 0x2e
#define SEI_ASM 0x2f
#define STA_ASM 0x30
#define STX_ASM 0x31
#define STY_ASM 0x32
#define TAX_ASM 0x33
#define TAY_ASM 0x34
#define TSX_ASM 0x35
#define TXA_ASM 0x36
#define TXS_ASM 0x37
#define TYA_ASM 0x38

struct asm6502 {
  int type;
  struct mem_addr maddr;
};

typedef struct asm6502 asm6502;

struct asm6502 asm6502_create(int type, struct mem_addr addr);

// Handlers

// Arithmetic 
void add_with_carry(state6502 *state, asm6502 cmd);
void increment_memory(state6502 *state, asm6502 cmd);
void increment_x(state6502 *state, asm6502 cmd);
void increment_y(state6502 *state, asm6502 cmd);
void subtract_with_carry(state6502 *state, asm6502 cmd);
void decrement_memory(state6502 *state, asm6502 cmd);
void decrement_x(state6502 *state, asm6502 cmd);
void decrement_y(state6502 *state, asm6502 cmd);

// Flags
void clear_carry(state6502 *state, asm6502 cmd);
void set_carry(state6502 *state, asm6502 cmd);

// Memory
void load_accumulator(state6502 *state, asm6502 cmd);
void load_xreg(state6502 *state, asm6502 cmd);
void load_yreg(state6502 *state, asm6502 cmd);
