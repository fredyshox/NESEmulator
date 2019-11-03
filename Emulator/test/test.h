//
// Common testing stuff
// Testing utilities used before I discovered gtest...
//

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ASSERT_T(EXPR, MSG, RES)  *RES = (EXPR); \
                                  if (EXPR) \
                                    fprintf(stderr, "\033[0;32mTest passed: %s\033[0m\n", MSG); \
                                  else \
                                    fprintf(stderr, "\033[0;31mTest failed: %s\033[0m\n", MSG);

// with block
#define ASSERT_T_DO(EXPR, MSG, RES) ASSERT_T(EXPR, MSG, RES); \
                                    if (EXPR)

// expected & actual as ints
#define ASSERT_EQ(EXPECTED, ACTUAL, MSG, RES) *RES = (EXPECTED == ACTUAL); \
                                              if (*RES) \
                                                fprintf(stderr, "\033[0;32mTest passed: %s\033[0m\n", MSG); \
                                              else \
                                                fprintf(stderr, "\033[0;31mTest failed: %s: expected %d, actual %d\033[0m\n", MSG, EXPECTED, ACTUAL);

// with block
#define ASSERT_EQ_DO(EXPECTED, ACTUAL, MSG, RES) ASSERT_EQ(EXPECTED, ACTUAL, MSG, RES); \
                                                 if (*RES)

#endif /* TEST_H */
