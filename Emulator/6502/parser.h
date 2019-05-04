//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <stdint.h>
#include "6502/asm.h"
#include "6502/addr.h"
#include "6502/state.h"
#include "6502/handlers/arithmetic.h"
#include "6502/handlers/branch.h"
#include "6502/handlers/memory.h"
#include "6502/handlers/other.h"
#include "6502/handlers/stack.h"

int execute_asm(state6502 *state);
int parse_asm(asm6502 *cmd, memory6502 *memory, uint16_t pos);

#endif /* parser_h */
