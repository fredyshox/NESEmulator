//
// iNES mapper 1
//

#include "nes/mappers/mapper1.h"

struct mapper* mapper1_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 1;
  m->cartridge = cartridge;
  m->read = mapper1_read;
  m->write = mapper1_write;

  return m;
}

// TODO
void mapper1_read(struct mapper* m, uint16_t address, uint8_t* dest) {}
void mapper1_write(struct mapper* m, uint16_t address, uint8_t byte) {}