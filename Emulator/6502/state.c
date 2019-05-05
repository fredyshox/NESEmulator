//
// Struct definitions for representing cpu state
// Copyright (c) 2019 Kacper Rączy
//

#include "6502/state.h"

void state6502_create(struct state6502 *state, struct memory6502 *memory) {
  assert(memory != NULL && state != NULL);

  state->reg_a = 0x00;
  state->reg_x = 0x00;
  state->reg_y = 0x00;
  state->sp = 0xff;
  state->pc = 0x0000;
  state->incoming_int = NONE_INT;
  state->status.byte = 0x00;
  state->memory = memory;
}

void state6502_request_interrupt(struct state6502* state, enum interrupt6502 i) {
  state->incoming_int = i;
}

void interrupt6502_handle(struct state6502 *state) {
  enum interrupt6502 intnum = state->incoming_int;
  uint16_t vec_addr;

  if ((state->status.int_disable == 1 && intnum != NMI_INT) || intnum == NONE_INT)
    return;

  if (intnum == RST_INT) {
    vec_addr = STATE6502_RESET_VECTOR;
  } else {
    if (intnum == IRQ_INT || intnum == BRK_INT)
      vec_addr = STATE6502_IRQ_VECTOR;
    else if (intnum == NMI_INT)
      vec_addr = STATE6502_NMI_VECTOR;

    // TODO check endianess
    STATE6502_STACK_PUSH(state, (uint8_t*) &state->pc, 2);
    STATE6502_STACK_PUSH(state, &state->status.byte, 1);
  }

  uint16_t vector = (uint16_t) memory6502_load(state->memory, vec_addr) | ((uint16_t) memory6502_load(state->memory, vec_addr + 1) << 8);
  state->status.int_disable = 1; // ?
  state->pc = vector;
}

void memory6502_create(struct memory6502 *memory, uint16_t size) {
  uint8_t *buffer = malloc(size * sizeof(uint8_t));
  memory->mptr = buffer;
  memory->size = size;
  memory->io = NULL;
  memory->store_handler = NULL;
  memory->load_handler = NULL;
}

void memory6502_store(struct memory6502 *memory, uint16_t idx, uint8_t value) {
  if (memory->store_handler == NULL) {
    // default
    if (idx >= memory->size) {
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
      return 0;
    }

    return memory->mptr[idx];
  }

  return memory->load_handler(memory, idx);
}
