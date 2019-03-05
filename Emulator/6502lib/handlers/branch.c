//
// 6502 asm branch handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "branch.h"
#include "addr.h"

#define BRANCH_IF(condition) if (!condition) { \
                                  state->pc += BRANCH_SIZE; \
                                } else { \
                                  uint16_t addr = handle_addr(state, cmd.maddr); \
                                  state->pc = addr; \
                                }

void branch_plus(state6502* state, asm6502 cmd) {
  BRANCH_IF(!state->status.negative);
}

void branch_minus(state6502* state, asm6502 cmd) {
  BRANCH_IF(state->status.negative);
}

void branch_carry(state6502* state, asm6502 cmd) {
  BRANCH_IF(state->status.carry);
}

void branch_carry_c(state6502* state, asm6502 cmd) {
  BRANCH_IF(!state->status.carry);
}

void branch_overflow(state6502* state, asm6502 cmd) {
  BRANCH_IF(state->status.overflow);
}

void branch_overflow_c(state6502* state, asm6502 cmd) {
  BRANCH_IF(!state->status.overflow);
}

void branch_eq(state6502* state, asm6502 cmd) {
  BRANCH_IF(state->status.zero);
}

void branch_neq(state6502* state, asm6502 cmd) {
  BRANCH_IF(!state->status.zero);
}

void jump(state6502* state, asm6502 cmd) {
  uint16_t addr = handle_addr(state, cmd.maddr);
  state->pc = addr;
}

void jump_subroutine(state6502* state, asm6502 cmd) {
  uint16_t next = state->pc + JUMP_SIZE;
  uint16_t addr = handle_addr(state, cmd.maddr);
  // push next - 1 on stack
  // TODO
  state->pc = addr;
}

void return_subroutine(state6502* state, asm6502 cmd) {
  // TODO
}
