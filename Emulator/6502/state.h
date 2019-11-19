//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#ifndef state_h
#define state_h

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/debug.h"

#ifdef DEBUG
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

struct memory6502;
struct state6502;

union flags6502 {
  struct {
    uint8_t carry : 1;
    uint8_t zero : 1;
    uint8_t int_disable : 1;
    uint8_t decimal : 1;
    uint8_t bflag4 : 1;
    uint8_t bflag5 : 1;
    uint8_t overflow : 1;
    uint8_t negative : 1;
  };
  uint8_t byte;
};

typedef union flags6502 flags6502;

enum interrupt6502 {
  NONE_INT,
  IRQ_INT,
  BRK_INT,
  NMI_INT,
  RST_INT
};

typedef enum interrupt6502 interrupt6502;

void interrupt6502_handle(struct state6502 *state);

struct state6502 {
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;
  uint8_t sp;
  uint16_t pc;
  union flags6502 status;
  struct memory6502 *memory;
  // interrupts
  enum interrupt6502 incoming_int;
  // temp data
  bool page_crossed;
  bool branch_taken;
};

typedef struct state6502 state6502;

void state6502_create(struct state6502 *state, struct memory6502 *memory);
void state6502_free(struct state6502* state);
void state6502_request_interrupt(struct state6502* state, enum interrupt6502 i);

#define STATE6502_NMI_VECTOR (0xfffa)
#define STATE6502_RESET_VECTOR (0xfffc)
#define STATE6502_IRQ_VECTOR (0xfffe)

#define STATE6502_STACK_PUSH(STATE, BYTES, LEN) \
  for (int i = 0; i < LEN; i++) {  \
    memory6502_store(STATE->memory, (uint16_t) STATE->sp + 0x100, *(BYTES + i));  \
    STATE->sp -= 1;  \
  }

#define STATE6502_STACK_PULL(STATE, BYTES, LEN) \
  for (int i = LEN - 1; i >= 0; i--) {  \
    STATE->sp += 1;  \
    *(BYTES + i) = memory6502_load(STATE->memory, (uint16_t) STATE->sp + 0x100);  \
  }

#define MAX_MEM_SIZE UINT16_MAX

struct memory6502 {
  uint8_t *mptr;
  uint16_t size;
  void *io;
  uint8_t (*load_handler)(struct memory6502*, uint16_t);
  void (*store_handler)(struct memory6502*, uint16_t, uint8_t);
};

typedef struct memory6502 memory6502;

void memory6502_create(struct memory6502 *memory, uint16_t size);
void memory6502_free(struct memory6502 *memory);
void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value);
uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx);

#endif /* state_h */
