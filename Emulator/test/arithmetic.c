//
// Arithmetic operations tests
// Copyright (c) 2019 Kacper Rączy
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
uint8_t adc_case[] = {
  /* .org(0x0000) */
  /* ADC */
};

void test_adc() {
  // carry flag
  // overflow flag 
  // etc
}

void test_sbc() {
  // same as adc
}

void test_inc() {

}

void test_dec() {

}

int main() {
  test_adc();
  test_sbc();
  test_inc();
  test_dec();
  return 0;
}

