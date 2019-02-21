//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#include "state.h"

void state6502_create(struct state6502 *state, struct memory6502 *memory) {
  assert(memory != NULL && state != NULL);
   
  state->reg_a = 0x00;
  state->reg_x = 0x00;
  state->reg_y = 0x00;
  state->sp = 0xff;
  state->pc = 0x0000;
  state->status = { 0x00 };
  state->memory = memory;
}

void memory6502_create(struct memory6502 *memory, uint16_t size) {
  uint8_t buffer = malloc(size * sizeof(uint8_t));
  memory->mptr = buffer;
  memory->size = size;
  memory->did_load_cb = NULL;
  memory->did_store_cb = NULL;
  memory->load_error_cb = NULL;
  memory->store_error_cb = NULL;
}

void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value) {
  if (idx >= memory->size) {
    __STORE_ERR_NNULL(memory, idx, value);
    return;
  }

  memory->mptr[idx] = value;
  __DID_STORE_NNULL(memory, idx, value);
}

uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx) {
  if (idx >= memory->size) {
    __LOAD_ERR_NNULL(memory, idx);
    return;
  }

  __DID_LOAD_NNULL(memory, idx);
  return memory->mptr[idx];
}

