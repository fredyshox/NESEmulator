//
// Common testing stuff
//

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "parser.h"
#include "state.h"  

#define ASSERT_T(EXPR, MSG, RES)  *RES = (EXPR); \
                                  if (EXPR) \
                                    fprintf(stderr, "\033[0;32mTest passed: %s\033[0m\n", MSG); \
                                  else \
                                    fprintf(stderr, "\033[0;31mTest failed: %s\033[0m\n", MSG);

#define TEST_PREPARE(CPU, PROGRAM) memset(&CPU, 0, sizeof(state6502)); \
                                   memory6502 memory = {0, 0, NULL, NULL, NULL, NULL}; \
                                   memory.mptr = PROGRAM; \
                                   memory.size = sizeof(PROGRAM) / sizeof(uint8_t); \
                                   cpu.memory = &memory; 

