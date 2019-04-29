//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_h
#define asm_h

#include "addr.h"

#define ASM_ARITHMETIC(CMD) ((CMD >> 8) == 0)
#define ASM_BITWISE(CMD)    ((CMD >> 8) == 1)
#define ASM_FLAGS(CMD)      ((CMD >> 8) == 2)
#define ASM_MEMORY(CMD)     ((CMD >> 8) == 3)
#define ASM_BRANCH(CMD)     ((CMD >> 8) == 4)
#define ASM_JUMP(CMD)       ((CMD >> 8) == 5)
#define ASM_STACK(CMD)      ((CMD >> 8) == 6)
#define ASM_OTHER(CMD)      ((CMD >> 8) == 7)

enum {
  // Arithmetic
  ADC_ASM = 0x001,
  DEC_ASM = 0x015,
  DEX_ASM = 0x016,
  DEY_ASM = 0x017,
  INC_ASM = 0x019,
  INX_ASM = 0x01a,
  INY_ASM = 0x01b,
  SBC_ASM = 0x02c,
  // Bitwise
  AND_ASM = 0x102,
  ASL_ASM = 0x103,
  BIT_ASM = 0x107,
  EOR_ASM = 0x118,
  LSR_ASM = 0x121,
  ORA_ASM = 0x123,
  ROL_ASM = 0x128,
  ROR_ASM = 0x129,
  // Flags
  CLC_ASM = 0x20e,
  CLD_ASM = 0x20f,
  CLI_ASM = 0x210,
  CLV_ASM = 0x211,
  SEC_ASM = 0x22d,
  SED_ASM = 0x22e,
  SEI_ASM = 0x22f,
  // Memory
  LDA_ASM = 0x31e,
  LDX_ASM = 0x31f,
  LDY_ASM = 0x320,
  STA_ASM = 0x330,
  STX_ASM = 0x331,
  STY_ASM = 0x332,
  TAX_ASM = 0x333,
  TAY_ASM = 0x334,
  TXA_ASM = 0x336,
  TYA_ASM = 0x338,
  // Branch
  BCC_ASM = 0x404,
  BCS_ASM = 0x405,
  BEQ_ASM = 0x406,
  BMI_ASM = 0x408,
  BNE_ASM = 0x409,
  BPL_ASM = 0x40a,
  BVC_ASM = 0x40c,
  BVS_ASM = 0x40d,
  // Jump
  JMP_ASM = 0x51c,
  JSR_ASM = 0x51d,
  RTI_ASM = 0x52a,
  RTS_ASM = 0x52b,
  BRK_ASM = 0x50b,
  // Stack
  PHA_ASM = 0x624,
  PHP_ASM = 0x625,
  PLA_ASM = 0x626,
  PLP_ASM = 0x627,
  TSX_ASM = 0x635,
  TXS_ASM = 0x637,
  // Other
  CMP_ASM = 0x712,
  CPX_ASM = 0x713,
  CPY_ASM = 0x714,
  NOP_ASM = 0x722,
};

struct asm6502 {
  int type;
  int cycles;
  struct mem_addr maddr;
  void (*handler)(struct state6502*, struct asm6502);
};

typedef struct asm6502 asm6502;

struct asm6502 asm6502_create(int type, struct mem_addr addr, void (*handler)(struct state6502*, struct asm6502));
void asm6502_execute(struct asm6502 cmd, struct state6502* state);

// Helpers
static inline void eval_zero_flag(state6502 *state, uint16_t value) {
  if ((value & 0xff) == 0) {
    state->status.zero = 1;
  } else {
    state->status.zero = 0;
  }
}

static inline void eval_sign_flag(state6502 *state, uint16_t value) {
  if (value & 0x80) {
    state->status.negative = 1;
  } else {
    state->status.negative = 0;
  }
}

static inline void eval_carry_flag(state6502 *state, uint16_t value) {
  if (value > 0xff) {
    state->status.carry = 1;
  } else {
    state->status.carry = 0;
  }
}

// Portable conversions

static inline void uint_16_to_8(uint16_t src, uint8_t *dest) {
  dest[0] = (uint8_t) (src >> 8) & 0xff;
  dest[1] = (uint8_t) (src) & 0xff;
}

#endif /* asm_h */
