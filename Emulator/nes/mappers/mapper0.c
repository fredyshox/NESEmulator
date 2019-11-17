//
// iNES mapper 0
//

#include "nes/mappers/mapper0.h"

struct mapper* mapper0_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 0;
  m->cartridge = cartridge;
  m->data = (void*) calloc(0x2000, sizeof(uint8_t));
  m->read = mapper0_read;
  m->write = mapper0_write;

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
    uint8_t* pgr_ram = (uint8_t*) m->data;
    *dest = pgr_ram[address - 0x6000];
  } else {
    debug_print("Warning: Mapper0 read at %x!\n", address);
  }
}

void mapper0_write(struct mapper* m, uint16_t address, uint8_t byte) {
  if (address < 0x2000 && m->cartridge->chr_ram_mode) {
    m->cartridge->chr_rom[address] = byte;
  } else if (address >= 0x6000 && address < 0x8000) {
    uint8_t* pgr_ram = (uint8_t*) m->data;
    pgr_ram[address - 0x6000] = byte; 
  } else {
    debug_print("Warning: Mapper0 write at %x!\n", address); 
  }
}
