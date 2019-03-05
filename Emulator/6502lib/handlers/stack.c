//
// 6502 stack instruction handlers
// Copyright (c) 2019 Kacper Rączy
//

#include "stack.h"
#include "addr.h"

void transfer_x2sptr(state6502 *state, asm6502 cmd) {
  state->sp = state->reg_x;
}

void transfer_sptr2x(state6502 *state, asm6502 cmd) {
  state->reg_x = state->sp;
}

void push_accumulator(state6502 *state, asm6502 cmd) {
  uint16_t addr = (uint16_t) state->sp + 0x100;
  memory6502_store(state->memory, addr, state->reg_a);
  state->sp -= 1;
}

void pull_accumulator(state6502 *state, asm6502 cmd) {
  uint16_t addr = (uint16_t) state->sp + 0x100;
  state->reg_a = memory6502_load(state->memory, addr);
  eval_zero_flag(state, (uint16_t) state->reg_a);
  eval_sign_flag(state, (uint16_t) state->reg_a);
  state->sp += 1;
}

void push_cpu_status(state6502 *state, asm6502 cmd) {
  uint16_t addr = (uint16_t) state->sp + 0x100;
  uint8_t cpu_status = *((uint8_t*) &state->status);
  memory6502_store(state->memory, addr, cpu_status);
  state->sp -= 1;
}

void pull_cpu_status(state6502 *state, asm6502 cmd) {
  uint16_t addr = (uint16_t) state->sp + 0x100;
  uint8_t cpu_status = memory6502_load(state->memory, addr);
  state->status = *((flags6502*) &cpu_status);
  state->sp += 1;
}
