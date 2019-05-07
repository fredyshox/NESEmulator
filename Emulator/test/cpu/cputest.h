//
// Common cpu testing stuff
//

#ifndef CPUTEST_H
#define CPUTEST_H

#include "test.h"
#include "6502/parser.h"
#include "6502/state.h"

#define TEST_PREPARE(CPU, PROGRAM) memset(&CPU, 0, sizeof(state6502)); \
                                   memory6502 memory = {0, 0, NULL, NULL, NULL}; \
                                   memory.mptr = PROGRAM; \
                                   memory.size = sizeof(PROGRAM) / sizeof(uint8_t); \
                                   cpu.memory = &memory;

#define PRINT_STATUS(CPU, DEVICE) fprintf(DEVICE, "CPU N: %d, ", CPU.status.negative); \
                                  fprintf(DEVICE, "V: %d, ", CPU.status.overflow); \
                                  fprintf(DEVICE, "B: %d, ", CPU.status.brk); \
                                  fprintf(DEVICE, "D: %d, ", CPU.status.decimal); \
                                  fprintf(DEVICE, "I: %d, ", CPU.status.int_disable); \
                                  fprintf(DEVICE, "Z: %d, ", CPU.status.zero); \
                                  fprintf(DEVICE, "C: %d\n", CPU.status.carry);

#endif /* CPUTEST_H */
