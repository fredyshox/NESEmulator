//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <stdint.h>
#include "instruction.h"
#include "addr.h"
#include "state.h"

int execute_asm(state6502 *state);
int parse_asm(asm6502 *cmd, memory6502 *memory, uint16_t pos);

#endif /* parser_h */
