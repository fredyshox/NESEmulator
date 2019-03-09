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
  memset(&state->status, 0x00, sizeof(struct flags6502));
  state->memory = memory;
}

void handle_interrupt(state6502 *state, interrupt6502 intnum) {
  uint16_t vec_addr;
  if (intnum == RST_INT) {
    vec_addr = 0xfffc;
    state->status.interrupt = 1;
  } else {
    if (intnum == IRQ_INT || intnum == BRK_INT)
      vec_addr = 0xfffe;
    else if (intnum == NMI_INT)
      vec_addr = 0xfffa;

    state->status.interrupt = 1;
    // TODO check endianess
    STATE6502_STACK_PUSH(state, (uint8_t*) &state->pc, 2);
    STATE6502_STACK_PUSH(state, (uint8_t*) &state->status, 1);
  }

  uint16_t vector = (uint16_t) memory6502_load(state->memory, vec_addr) | ((uint16_t) memory6502_load(state->memory, vec_addr + 1) << 8);
  state->pc = vector;
}

void memory6502_create(struct memory6502 *memory, uint16_t size) {
  uint8_t *buffer = malloc(size * sizeof(uint8_t));
  memory->mptr = buffer;
  memory->size = size;
  memory->store_handler = NULL;
  memory->load_handler = NULL;
  memory->load_error_cb = NULL;
  memory->store_error_cb = NULL;
}

void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value) {
  if (memory->store_handler == NULL) {
    // default
    if (idx >= memory->size) {
      __STORE_ERR_NNULL(memory, idx, value);
      return;
    }

    memory->mptr[idx] = value;
    return;
  }

  memory->store_handler(memory, idx, value);
}

uint8_t memory6502_load(struct memory6502 *memory, uint16_t idx) {
  if (memory->load_handler == NULL) {
    // default
    if (idx >= memory->size) {
      return __LOAD_ERR_NNULL(memory, idx);
    }

    return memory->mptr[idx];
  }

  return memory->load_handler(memory, idx);
}
