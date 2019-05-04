//
// 6502 asm memory handlers
// Copyright (c) 2019 Kacper Rączy
//

#ifndef asm_memory_h
#define asm_memory_h

#include "6502/asm.h"
#include "6502/state.h"

void load_accumulator(state6502 *state, asm6502 cmd);
void load_xreg(state6502 *state, asm6502 cmd);
void load_yreg(state6502 *state, asm6502 cmd);
void store_accumulator(state6502 *state, asm6502 cmd);
void store_xreg(state6502 *state, asm6502 cmd);
void store_yreg(state6502 *state, asm6502 cmd);
void transfer_a2x(state6502 *state, asm6502 cmd);
void transfer_x2a(state6502 *state, asm6502 cmd);
void transfer_a2y(state6502 *state, asm6502 cmd);
void transfer_y2a(state6502 *state, asm6502 cmd);

#endif /* asm_memory_h */
