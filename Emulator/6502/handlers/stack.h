//
// 6502 stack instruction handlers
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_stack_h
#define asm_stack_h

#include "asm.h"
#include "state.h"

struct asm6502;

void transfer_x2sptr(state6502 *state, asm6502 cmd);
void transfer_sptr2x(state6502 *state, asm6502 cmd);
void push_accumulator(state6502 *state, asm6502 cmd);
void pull_accumulator(state6502 *state, asm6502 cmd);
void push_cpu_status(state6502 *state, asm6502 cmd);
void pull_cpu_status(state6502 *state, asm6502 cmd);

#endif /* asm_stack_h */
