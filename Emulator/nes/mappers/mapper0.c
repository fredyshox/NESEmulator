//
// iNES mapper 0
//

#include "nes/mappers/mapper0.h"

mapper* mapper0_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 0;
  m->cartridge = cartridge;
  m->data = NULL;
  m->read = mapper0_read;
  m->write = NULL;

  return m;
}

void mapper0_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  if (address < 0x2000) {
    // chr rom - ppu pattern tables
    if (m->cartridge->chr_rom_size > address) {
      *dest = m->cartridge->chr_rom[address];
    } else {
      debug_print("Error: Mapper0 CHR rom address out of bounds: %x\n", address);
    }
  } else if (address >= 0x8000) {
    // prg rom
    if (m->cartridge->prg_rom_size <= 0x4000) {
      // 16KB bank
      uint16_t offset = (address < 0xc000) ? 0x8000 : 0xc000;
      *dest = m->cartridge->prg_rom[address - offset];
    } else {
      // 32KB bank
      *dest = m->cartridge->prg_rom[address - 0x8000];
    }
  } else if (address >= 0x6000) {
    //TODO prg ram
  } else {
    debug_print("Warning: Mapper0 read at %x!\n", address);
  }
}
