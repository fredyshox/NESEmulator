//
// Common cpu testing stuff
//

#ifndef CPUTEST_H
#define CPUTEST_H

#include "test.h"
#include "parser.h"
#include "state.h"

#define TEST_PREPARE(CPU, PROGRAM) memset(&CPU, 0, sizeof(state6502)); \
                                   memory6502 memory = {0, 0, NULL, NULL, NULL, NULL}; \
                                   memory.mptr = PROGRAM; \
                                   memory.size = sizeof(PROGRAM) / sizeof(uint8_t); \
                                   cpu.memory = &memory; 

#endif /* CPUTEST_H */
