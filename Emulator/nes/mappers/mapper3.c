//
// iNES Mapper 3
// CNROM
//

#include "nes/mappers/mapper3.h"

mapper* mapper3_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 3;
  m->cartridge = cartridge;
  m->data = (void*) malloc(sizeof(int));
  m->write = mapper3_write;
  m->read = mapper3_read;

  return m;
}

void mapper3_write(struct mapper* m, uint16_t address, uint8_t byte) {
  // <= 0xffff inferred
  if (address >= 0x8000) {
    int* bank_ptr = (int*) m->data;
    *bank_ptr = (int) (byte & 0x03);
  } else {
    fprintf(stderr, "Warning: Mapper3 write at %d!\n", address);
  }
}

void mapper3_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  if (address < 0x2000) {
    // chr rom
    int bank = *(int*) m->data;
    uint16_t chr_addr = (uint16_t) bank * address;
    if (chr_addr < m->cartridge->chr_rom_size) {
      *dest = m->cartridge->chr_rom[chr_addr];
    } else {
      fprintf(stderr, "Error: Mapper3 CHR rom address out of bounds: %x\n", chr_addr);
    }
  } else if (address >= 0x8000) {
    // prg rom
    if (m->cartridge->prg_rom_size <= 0x4000) {
      // 16KB bank
      uint16_t offset = (address < 0xc000) ? 0x8000 : 0xc000;
      *dest = m->cartridge->prg_rom[address - offset];
    } else {
      // 32KB bank
      *dest = m->cartridge->prg_rom[address];
    }
  } else {
    fprintf(stderr, "Warning: Mapper3 read at %x!\n", address);
  }
}
