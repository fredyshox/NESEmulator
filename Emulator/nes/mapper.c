//
// Abstract nes mapper
// Copyright (c) 2019 Kacper Rączy
//

#include "nes/mapper.h"
#include <stdio.h>

void mapper_write(struct mapper* m, uint16_t address, uint8_t byte) {
  if (m->write != NULL)
    m->write(m, address, byte);
}

void mapper_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  m->read(m, address, dest);
}

void mapper_free(struct mapper* m) {
  free(m->data);
}

// mappers
#include "nes/mappers/mapper0.h"
#include "nes/mappers/mapper2.h"
#include "nes/mappers/mapper3.h"

struct mapper* mapper_from_id(struct cartridge* c, int id) {
  switch (id) {
    case 0:
      return mapper0_create(c);
    case 2:
      return mapper2_create(c);
    case 3:
      return mapper3_create(c);
    default:
      debug_print("Error: Mapper %d is not supported!\n", id);
      return NULL;
  }
}
