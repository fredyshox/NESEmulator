//
// iNES mapper 1
//

#include "nes/mappers/mapper1.h"
#include "ppu/ppu.h"

struct mapper* mapper1_create(struct cartridge* cartridge) {
  mapper* m = malloc(sizeof(mapper));
  m->id = 1;
  m->cartridge = cartridge;
  m->data = (void*) calloc(1, sizeof(struct mapper1_data));
  m->read = mapper1_read;
  m->write = mapper1_write;

  return m;
}

void mapper1_data_copy_cache(struct mapper1_data* data, uint8_t reg_index) {
  uint8_t cache = data->cache & 0x1f;
  switch (reg_index) {
    case 0:
      data->control = cache;
      break;
    case 1:
      data->chr_bank0 = cache;
      break;
    case 2:
      data->chr_bank1 = cache;
      break;
    case 3:
      data->prg_bank = cache;
      break;
    default: break;
  }
}

void mapper1_switch_mirroring(struct mapper* m, uint8_t control) {
  switch (control & 0x03) {
    case 0:
    case 1:
      m->mirroring_type = PPU_MIRRORING_SINGLE_SCREEN;
      break;
    case 2:
      m->mirroring_type = PPU_MIRRORING_VERTICAL;
      break;
    case 3:
      m->mirroring_type = PPU_MIRRORING_HORIZONTAL;
      break;
    default: break;
  }
}

void mapper1_write(struct mapper* m, uint16_t address, uint8_t byte) {  
  if (address >= 0x8000) {
    struct mapper1_data* data = (struct mapper1_data*) m->data;
    if (byte & 0x80) {
      data->cache = 0x00;
      data->counter = 0;
      return; 
    }

    data->cache |= (byte & 0x01) << data->counter;
    data->counter += 1;
    if (data->counter == 5) {
      mapper1_data_copy_cache(data, (uint8_t) ((address >> 13) & 0x03));
      mapper1_switch_mirroring(m, data->control);
      data->cache = 0x00;
      data->counter = 0;
    }
  } else if (address >= 0x6000) {
    m->cartridge->prg_ram[address - 0x6000] = byte;
  } else {
    debug_print("Warning: Mapper1 write at %x!\n", address);
  }
}

void mapper1_read(struct mapper* m, uint16_t address, uint8_t* dest) {
  struct mapper1_data* data = (struct mapper1_data*) m->data;
  if (address < 0x2000) {
    if ((data->control >> 4) & 0x01) {
      // 4 KB mode
      address &= 0xfff;
      uint8_t bank = address >= 0x1000 ? data->chr_bank1 : data->chr_bank0;
      *dest = m->cartridge->chr_rom[bank * 0x1000 + address];
    } else {
      // 8 KB mode
      uint8_t bank = (data->chr_bank0 >> 1);
      *dest = m->cartridge->chr_rom[bank * 0x2000 + address];
    }
  } else if (address >= 0x8000) {
    address -= 0x8000;
    uint8_t prg_mode = (data->control >> 2) & 0x03;
    if (prg_mode == 0 || prg_mode == 1) {
      int bank = (int) (data->prg_bank >> 1) & 0x07;
      *dest = m->cartridge->prg_rom[bank * 0x8000 + address];
    } else if (prg_mode == 2) {
      int bank = (int) data->prg_bank & 0x0f;
      if (address < 0xc000) {
        *dest = m->cartridge->prg_rom[address]; 
      } else {
        *dest = m->cartridge->prg_rom[bank * 0x4000 + (address - 0x4000)];
      }
    } else { // == 3
      int bank = (int) data->prg_bank & 0x0f;
      if (address < 0xc000) {
        *dest = m->cartridge->prg_rom[bank * 0x4000 + address];
      } else {
        int offset = m->cartridge->prg_rom_size - 0x4000; // last bank
        *dest = m->cartridge->prg_rom[offset + (address - 0x4000)];
      }
    }
  } else if (address >= 0x6000) {
    *dest = m->cartridge->prg_ram[address - 0x6000];
  } else {
    debug_print("Warning: Mapper1 read at %x!\n", address);
  }
}
