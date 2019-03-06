//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#ifndef state_h
#define state_h

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct memory6502;

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
  struct memory6502 *memory;
};

typedef struct state6502 state6502;

void state6502_create(struct state6502 *state, struct memory6502 *memory);

#define STATE6502_STACK_PUSH(STATE, BYTES, LEN) \
  for (int i = 0; i < LEN; i++) {  \
    memory6502_store(STATE->memory, (uint16_t) STATE->sp + 0x100, *(BYTES + i));  \
    STATE->sp -= 1;  \
  }

#define STATE6502_STACK_PULL(STATE, BYTES, LEN) \
  for (int i = LEN - 1; i >= 0; i--) {  \
    *(BYTES + i) = memory6502_load(STATE->memory, (uint16_t) STATE->sp + 0x100);  \
    STATE->sp += 1;  \
  }  \

#define MAX_MEM_SIZE UINT16_MAX

struct memory6502 {
  uint8_t *mptr;
  uint16_t size;
  uint8_t (*load_handler)(struct memory6502*, uint16_t);
  void (*store_handler)(struct memory6502*, uint16_t, uint8_t);
  uint8_t (*load_error_cb)(struct memory6502*, uint16_t);
  void (*store_error_cb)(struct memory6502*, uint16_t, uint8_t);
};

typedef struct memory6502 memory6502;

#define __STORE_ERR_NNULL(mem, idx, val) if (mem->store_error_cb != NULL) mem->store_error_cb(mem, idx, val);
#define __LOAD_ERR_NNULL(mem, idx) (mem->load_error_cb != NULL) ? mem->load_error_cb(mem, idx) : 0x00

void memory6502_create(struct memory6502 *memory, uint16_t size);
void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value);
uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx);

#endif /* state_h */
