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
#include "nes/mappers/mapper1.h"
#include "nes/mappers/mapper2.h"
#include "nes/mappers/mapper3.h"

struct mapper* mapper_from_id(struct cartridge* c, int id) {
  struct mapper* m;
  switch (id) {
    case 0:
      m = mapper0_create(c);
      break;
    case 1:
      m = mapper1_create(c);
      break;
    case 2:
      m = mapper2_create(c);
      break;
    case 3:
      m = mapper3_create(c);
      break;
    default:
      debug_print("Error: Mapper %d is not supported!\n", id);
      return NULL;
  }
  m->mirroring_type = c->mirroring_type;
  return m;
}
