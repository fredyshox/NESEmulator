//
// iNES mapper 2
//

#include "nes/mappers/mapper2.h"

struct mapper* mapper2_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 2;
  m->cartridge = cartridge;
  m->data = (void*) calloc(1, sizeof(uint8_t));
  m->write = mapper2_write;
  m->read = mapper2_read;

  return m;
}

void mapper2_write(struct mapper* m, uint16_t address, uint8_t byte) {
  if (address < 0x2000 && m->cartridge->chr_ram_mode) {
    m->cartridge->chr_rom[address] = byte;
  } else if (address >= 0x8000) {
    uint8_t* bank = (uint8_t*) m->data;
    if ((int) byte * 0x4000 < m->cartridge->prg_rom_size) {
      *bank = byte;   
    } else {
      debug_print("Warning: Mapper2 prg bank index exceed prg rom size %d\n", byte);
    }
  } else {
    debug_print("Warning: Mapper2 write at %x!\n", address);
  }
}

void mapper2_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  if (address < 0x2000) {
    *dest = m->cartridge->chr_rom[address];
  } else if (address >= 0x8000 && address < 0xc000) {
    int bank =  (int) *(uint8_t*) m->data;
    int prg_address = bank * 0x4000 + address - 0x8000;
    *dest = m->cartridge->prg_rom[prg_address];
  } else if (address >= 0xc000) {
    // last bank of prg rom 
    int offset = m->cartridge->prg_rom_size - 0x4000;
    *dest = m->cartridge->prg_rom[offset + (address - 0xc000)];
  } else {
    debug_print("Warning: Mapper2 read at %x!\n", address);
  }
}
