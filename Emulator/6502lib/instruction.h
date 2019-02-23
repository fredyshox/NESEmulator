//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#ifndef instruction_h
#define instruction_h

#include "addr.h"

enum {
  ADC_ASM = 0x01,
  AND_ASM = 0x02,
  ASL_ASM = 0x03,
  BCC_ASM = 0x04,
  BCS_ASM = 0x05,
  BEQ_ASM = 0x06,
  BIT_ASM = 0x07,
  BMI_ASM = 0x08,
  BNE_ASM = 0x09,
  BPL_ASM = 0x0a,
  BRK_ASM = 0x0b,
  BVC_ASM = 0x0c,
  BVS_ASM = 0x0d,
  CLC_ASM = 0x0e,
  CLD_ASM = 0x0f,
  CLI_ASM = 0x10,
  CLV_ASM = 0x11,
  CMP_ASM = 0x12,
  CPX_ASM = 0x13,
  CPY_ASM = 0x14,
  DEC_ASM = 0x15,
  DEX_ASM = 0x16,
  DEY_ASM = 0x17,
  EOR_ASM = 0x18,
  INC_ASM = 0x19,
  INX_ASM = 0x1a,
  INY_ASM = 0x1b,
  JMP_ASM = 0x1c,
  JSR_ASM = 0x1d,
  LDA_ASM = 0x1e,
  LDX_ASM = 0x1f,
  LDY_ASM = 0x20,
  LSR_ASM = 0x21,
  NOP_ASM = 0x22,
  ORA_ASM = 0x23,
  PHA_ASM = 0x24,
  PHP_ASM = 0x25,
  PLA_ASM = 0x26,
  PLP_ASM = 0x27,
  ROL_ASM = 0x28,
  ROR_ASM = 0x29,
  RTI_ASM = 0x2a,
  RTS_ASM = 0x2b,
  SBC_ASM = 0x2c,
  SEC_ASM = 0x2d,
  SED_ASM = 0x2e,
  SEI_ASM = 0x2f,
  STA_ASM = 0x30,
  STX_ASM = 0x31,
  STY_ASM = 0x32,
  TAX_ASM = 0x33,
  TAY_ASM = 0x34,
  TSX_ASM = 0x35,
  TXA_ASM = 0x36,
  TXS_ASM = 0x37,
  TYA_ASM = 0x38
};

struct asm6502 {
  int type;
  struct mem_addr maddr;
  void (*handler)(struct state6502*, struct asm6502);
};

typedef struct asm6502 asm6502;

struct asm6502 asm6502_create(int type, struct mem_addr addr, void (*handler)(struct state6502*, struct asm6502));
void asm6502_execute(struct asm6502 cmd, struct state6502* state); 
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

#endif /* instruction_h */
