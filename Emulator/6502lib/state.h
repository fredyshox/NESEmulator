//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#include <stdint.h>

struct flags6502 {
  uint8_t negative : 1;
  uint8_t overflow : 1;
  uint8_t unused : 1;
  uint8_t brk : 1;
  uint8_t decimal : 1;
  uint8_t interrupt : 1;
  uint8_t zero : 1;
  uint8_t carry : 1;
};

typedef struct flags6502 flags6502;

struct state6502 {
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;
  uint8_t sp;
  uint16_t pc;
  struct flags6502 status;
  uint8_t *memory;
};

typedef struct state6502 state6502;
