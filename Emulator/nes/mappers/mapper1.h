//
// iNES mapper 1
//
// prg switchable + fixed
// chr switchable
// vertical, horizontal or single-screen mirroring, switchable
// possible prg ram, switchable
//

#ifndef mapper1_h
#define mapper1_h

#include "nes/mapper.h"
#include <stdlib.h>
#include <stdio.h>

struct mapper1_data {
  int counter;
  uint8_t cache;
  uint8_t control;
  uint8_t chr_bank0;
  uint8_t chr_bank1;
  uint8_t prg_bank;
};

struct mapper* mapper1_create(struct cartridge* cartridge);
void mapper1_read(struct mapper* m, uint16_t address, uint8_t* dest);
void mapper1_write(struct mapper* m, uint16_t address, uint8_t byte);

#endif /* mapper1_h */
