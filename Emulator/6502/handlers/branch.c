//
// 6502 asm branch handlers
// Compilation handlers that directly modify program counter
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/handlers/branch.h"
#include "6502/addr.h"

#define BRANCH_IF(condition) if (condition) { \
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
  uint16_t next = state->pc - 1;
  uint16_t addr = handle_addr(state, cmd.maddr);
  // portable conversion
  uint8_t bytes[2];
  uint_16_to_8(next, bytes);
  STATE6502_STACK_PUSH(state, bytes, 2);
  state->pc = addr;
}

void return_subroutine(state6502* state, asm6502 cmd) {
  uint8_t lsb, msb;
  STATE6502_STACK_PULL(state, &lsb, 1);
  STATE6502_STACK_PULL(state, &msb, 1);
  uint16_t addr = (uint16_t) msb << 8 | (uint16_t) lsb;
  state->pc = addr + 1;
}

void return_interrupt(state6502* state, asm6502 cmd) {
  // cpu status (1 byte) + program counter (2 bytes)
  uint8_t data[3];
  STATE6502_STACK_PULL(state, data, 3);
  // ^pulls in reverse order
  state->pc = (uint16_t) data[0] << 8 | (uint16_t) data[1];
  state->status.byte = *(data + 2);
}

void break_interrupt(state6502 *state, asm6502 cmd) {
  state->status.brk = 1;
  state->incoming_int = BRK_INT;
}
