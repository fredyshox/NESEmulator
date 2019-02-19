//
// 6502 binary parser
// Copyright (c) 2019 Kacper Rączy
//

#include <stdint.h>
#include "instruction.h"
#include "addr.h"

int execute_asm(state6502 *state);
int parse_asm(asm6502 *cmd, uint8_t *bytes, int bcount);
