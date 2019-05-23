//
// 6502 asm instructions
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_h
#define asm_h

#include "6502/addr.h"

#define ASM_ARITHMETIC(CMD) ((CMD >> 8) == 0)
#define ASM_BITWISE(CMD)    ((CMD >> 8) == 1)
#define ASM_FLAGS(CMD)      ((CMD >> 8) == 2)
#define ASM_MEMORY(CMD)     ((CMD >> 8) == 3)
#define ASM_BRANCH(CMD)     ((CMD >> 8) == 4)
#define ASM_JUMP(CMD)       ((CMD >> 8) == 5)
#define ASM_STACK(CMD)      ((CMD >> 8) == 6)
#define ASM_OTHER(CMD)      ((CMD >> 8) == 7)

#define FOREACH_ASM(ASM)  \
  /* Arithmetic */  \
  ASM(ADC)  \
  ASM(DEC)  \
  ASM(DEX)  \
  ASM(DEY)  \
  ASM(INC)  \
  ASM(INX)  \
  ASM(INY)  \
  ASM(SBC)  \
  /* Bitwise */ \
  ASM(AND)  \
  ASM(ASL)  \
  ASM(BIT)  \
  ASM(EOR)  \
  ASM(LSR)  \
  ASM(ORA)  \
  ASM(ROL)  \
  ASM(ROR)  \
  /* Flags */   \
  ASM(CLC)  \
  ASM(CLD)  \
  ASM(CLI)  \
  ASM(CLV)  \
  ASM(SEC)  \
  ASM(SED)  \
  ASM(SEI)  \
  /* Memory */  \
  ASM(LDA)  \
  ASM(LDX)  \
  ASM(LDY)  \
  ASM(STA)  \
  ASM(STX)  \
  ASM(STY)  \
  ASM(TAX)  \
  ASM(TAY)  \
  ASM(TXA)  \
  ASM(TYA)  \
  /* Branch */  \
  ASM(BCC)  \
  ASM(BCS)  \
  ASM(BEQ)  \
  ASM(BMI)  \
  ASM(BNE)  \
  ASM(BPL)  \
  ASM(BVC)  \
  ASM(BVS)  \
  /* Jump */    \
  ASM(JMP)  \
  ASM(JSR)  \
  ASM(RTI)  \
  ASM(RTS)  \
  ASM(BRK)  \
  /* Stack */   \
  ASM(PHA)  \
  ASM(PHP)  \
  ASM(PLA)  \
  ASM(PLP)  \
  ASM(TSX)  \
  ASM(TXS)  \
  /* Other */   \
  ASM(CMP)  \
  ASM(CPX)  \
  ASM(CPY)  \
  ASM(NOP)  \

#define GENERATE_ENUM(ENUM) ENUM ## _ASM,
#define GENERATE_ASM_STRING(STRING) #STRING,

enum {
  FOREACH_ASM(GENERATE_ENUM)
};

extern const char* ASM_STRING[];

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
  dest[0] = (uint8_t) ((src >> 8) & 0xff);
  dest[1] = (uint8_t) ((src) & 0xff);
}

#endif /* asm_h */
