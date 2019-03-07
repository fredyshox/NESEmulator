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
  uint8_t value = state->reg_a;
  STATE6502_STACK_PUSH(state, &value, 1);
}

void pull_accumulator(state6502 *state, asm6502 cmd) {
  uint8_t acc;
  STATE6502_STACK_PULL(state, &acc, 1);
  state->reg_a = acc;
  eval_zero_flag(state, (uint16_t) state->reg_a);
  eval_sign_flag(state, (uint16_t) state->reg_a);
}

void push_cpu_status(state6502 *state, asm6502 cmd) {
  uint8_t cpu_status = *((uint8_t*) &state->status);
  STATE6502_STACK_PUSH(state, &cpu_status, 1);
}

void pull_cpu_status(state6502 *state, asm6502 cmd) {
  uint8_t cpu_status;
  STATE6502_STACK_PULL(state, &cpu_status, 1);
  state->status = *((flags6502*) &cpu_status);
}
