//
// NES console emulator
//

#include "nes/console.h"

// emulator setup

int setup_mapper(nes_t* console, cartridge* cartridge) {
  mapper* m = mapper_from_id(cartridge, cartridge->mapper);
  if (m == NULL) {
    return 1;
  }

  console->mapper = m;
  console->cartridge = cartridge;
  return 0;
}

int setup_cpu(nes_t* console) {
  state6502* cpu = malloc(sizeof(state6502));
  memory6502* mem = malloc(sizeof(memory6502));
  memory6502_create(mem, 0x800);
  mem->io = console;
  mem->load_handler = nes_cpu_memory_read;
  mem->store_handler = nes_cpu_memory_write;
  state6502_create(cpu, mem);
  console->cpu = cpu;

  return 0;
}

int setup_ppu(nes_t* console) {
  ppu_state* ppu = malloc(sizeof(ppu_state));
  ppu_memory* mem = malloc(sizeof(ppu_memory));
  ppu_render_handle* handle = ppu_render_handle_create();
  console->ppu_handle = handle;
  ppu_memory_create(mem);
  mem->io = console;
  mem->load_handler = nes_ppu_memory_read;
  mem->store_handler = nes_ppu_memory_write;
  ppu_state_create(ppu, mem);
  console->ppu = ppu;

  return 0;
}

int nes_create(nes_t* console) {
  int c, p, m;
  console->mapper = NULL;
  c = setup_cpu(console);
  p = setup_ppu(console);
  return (!c && !p);
}

void nes_free(nes_t* console) {
  //TODO
}

int nes_load_rom(nes_t* console, struct cartridge* cartridge) {
  int m;
  uint16_t reset_low, reset_high;
  struct ppu_memory* mem;
  // mapper
  m = setup_mapper(console, cartridge);
  // init cpu (set pc)
  reset_low = (uint16_t) memory6502_load(console->cpu->memory, STATE6502_RESET_VECTOR);
  reset_high = (uint16_t) memory6502_load(console->cpu->memory, STATE6502_RESET_VECTOR + 1);
  console->cpu->pc = (reset_high << 8) | reset_low;
  // init ppu (set nametable addr)
  mem = console->ppu->memory;
  switch (cartridge->mirroring_type) {
    case HORIZONTAL:
      mem->nametable0 = mem->nt_buf;
      mem->attrtable0 = (mem->nt_buf + PPU_NAMETABLE_SIZE);
      mem->nametable1 = mem->nametable0;
      mem->attrtable1 = mem->attrtable0;
      mem->nametable2 = (mem->nt_buf + PPU_NTAT_SIZE);
      mem->attrtable2 = (mem->nt_buf + PPU_NTAT_SIZE + PPU_NAMETABLE_SIZE);
      mem->nametable3 = mem->nametable2;
      mem->attrtable3 = mem->attrtable2;
      break;
    case VERTICAL:
      mem->nametable0 = mem->nt_buf;
      mem->attrtable0 = (mem->nt_buf + PPU_NAMETABLE_SIZE);
      mem->nametable1 = (mem->nt_buf + PPU_NTAT_SIZE);
      mem->attrtable1 = (mem->nt_buf + PPU_NTAT_SIZE + PPU_NAMETABLE_SIZE);
      mem->nametable2 = mem->nametable0;
      mem->attrtable2 = mem->attrtable0;
      mem->nametable3 = mem->nametable1;
      mem->attrtable3 = mem->attrtable1;
      break;
    case FOUR_SCREEN:
      mem->nametable0 = mem->nt_buf;
      mem->attrtable0 = (mem->nt_buf + PPU_NAMETABLE_SIZE);
      mem->nametable1 = (mem->nt_buf + PPU_NTAT_SIZE);
      mem->attrtable1 = (mem->nt_buf + PPU_NTAT_SIZE + PPU_NAMETABLE_SIZE);
      mem->nametable2 = (mem->nt_buf + 2*PPU_NTAT_SIZE);
      mem->attrtable2 = (mem->nt_buf + 2*PPU_NTAT_SIZE + PPU_NAMETABLE_SIZE);
      mem->nametable3 = (mem->nt_buf + 3*PPU_NTAT_SIZE);
      mem->attrtable3 = (mem->nt_buf + 3*PPU_NTAT_SIZE + PPU_NAMETABLE_SIZE);
      break;
    case SINGLE_SCREEN:
      mem->nametable0 = mem->nt_buf;
      mem->attrtable0 = (mem->nt_buf + PPU_NAMETABLE_SIZE);
      mem->nametable1 = mem->nametable0;
      mem->attrtable1 = mem->attrtable0;
      mem->nametable2 = mem->nametable0;
      mem->attrtable2 = mem->attrtable0;
      mem->nametable3 = mem->nametable0;
      mem->attrtable3 = mem->attrtable0;
      break;
  }

  return m;
}

bool nes_is_loaded(nes_t* console) {
  return (console->cartridge != NULL && console->mapper != NULL);
}

int nes_step(nes_t* console) {
  int cpu_cycles;
  cpu_cycles = execute_asm(console->cpu);
  for (int i = 0; i < cpu_cycles * 3; i++) {
    ppu_execute_cycle(console->ppu, console->ppu_handle);
  }

  if (console->ppu->status.vblank) {
    console->cpu->incoming_int = NMI_INT;
  }

  return cpu_cycles;
}

void nes_step_time(nes_t* console, double seconds) {
  double cpu_cycles = NES_CPU_FREQ * seconds;
  while (cpu_cycles > 0) {
    cpu_cycles -= nes_step(console);
  }
}

// memory handlers

uint8_t nes_cpu_memory_read(struct memory6502* memory, uint16_t addr) {
  nes_t* console = (nes_t*) memory->io;
  if (addr < 0x2000) {
    // ram is mirrored 3 times
    uint16_t ram_addr = addr % memory->size;
    return memory->mptr[ram_addr];
  } else if (addr < 0x4000) {
    // ppu io registers
    uint8_t out;
    switch ((addr % 8) + 0x2000) {
      case PPUSTATUS:
        ppu_status_read(console->ppu, &out);
        break;
      case OAMDATA:
        ppu_sr_data_read(console->ppu, &out);
        break;
      case PPUDATA:
        ppu_data_read(console->ppu, &out);
        break;
      default:
        fprintf(stderr, "Warning: CPU read access on %d\n", addr);
        out = 0;
        break;
    }

    return out;
  } else if (addr < 0x4020 || addr < 0x6000) {
    // other io
    return 0;
  } else {
    // rom stuff
    uint8_t out;
    mapper_read(console->mapper, addr, &out);
    return out;
  }
}

void nes_cpu_memory_write(struct memory6502* memory, uint16_t addr, uint8_t byte) {
  nes_t* console = (nes_t*) memory->io;
  if (addr < 0x2000) {
    uint16_t ram_addr = addr % memory->size;
    memory->mptr[ram_addr] = byte;
  } else if (addr < 0x4000) {
    switch ((addr % 8) + 0x2000) {
      case PPUCTRL:
        ppu_ctrl_write(console->ppu, byte);
        break;
      case PPUMASK:
        ppu_mask_write(console->ppu, byte);
        break;
      case OAMADDR:
        ppu_sr_addr_write(console->ppu, byte);
        break;
      case OAMDATA:
        ppu_sr_data_write(console->ppu, byte);
        break;
      case PPUSCROLL:
        ppu_scroll_write(console->ppu, byte);
        break;
      case PPUADDR:
        ppu_addr_write(console->ppu, byte);
        break;
      case PPUDATA:
        ppu_data_write(console->ppu, byte);
        break;
      default:
        fprintf(stderr, "Warning: CPU write access on %d\n", addr);
        break;
    }
  } else if (addr < 0x4020 || addr < 0x6000) {
    // io
    uint8_t oam[0xff];
    uint16_t base_addr = (uint16_t) byte << 8;
    switch (addr) {
      case OAMDMA:
        for (uint16_t i = 0; i <= 0xff; i++) {
          oam[i] = nes_cpu_memory_read(console->cpu->memory, base_addr + i);
        }
        ppu_sr_dma_write(console->ppu, oam, 0xff);
      default:
        fprintf(stderr, "Warning: Not implemented - write on %d\n", addr);
        break;
    }
  } else {
    mapper_write(console->mapper, addr, byte);
  }
}

uint8_t nes_ppu_memory_read(struct ppu_memory* memory, uint16_t addr) {
  nes_t* console = (nes_t*) memory->io;
  struct ppu_memory* mem = console->ppu->memory;

  if (addr >= 0x4000) {
    addr = addr % 0x4000;
  }

  if (addr < 0x2000) {
    struct mapper* mapper = (struct mapper*) console->mapper;
    uint8_t out;
    mapper_read(mapper, addr, &out);
    return out;
  } else if (addr < 0x3f00) {
    uint16_t base = addr < 0x3000 ? addr - 0x2000 : addr - 0x3000;
    uint16_t n = base % PPU_NTAT_SIZE;
    uint8_t *nt, *at;
    switch (base / PPU_NTAT_SIZE) {
      case 0:
        nt = mem->nametable0; at = mem->attrtable0;
        break;
      case 1:
        nt = mem->nametable1; at = mem->attrtable1;
        break;
      case 2:
        nt = mem->nametable2; at = mem->attrtable2;
        break;
      case 3:
        nt = mem->nametable3; at = mem->attrtable3;
        break;
      default: break; /* never gonna happen */
    }

    if (n < PPU_NAMETABLE_SIZE) {
      // nametable
      return nt[n];
    } else {
      // attrtable
      return at[n - PPU_NAMETABLE_SIZE];
    }
  } else if (addr < 0x4000) {
    uint16_t n = addr % (2*PPU_PALETTE_SIZE);
    if (n < 0x0f) {
      return mem->image_palette[n];
    } else {
      return mem->sprite_palette[n - 0x0f];
    }
  }

  return 0; // never gonna happen
}

void nes_ppu_memory_write(struct ppu_memory* memory, uint16_t addr, uint8_t byte) {
  nes_t* console = (nes_t*) memory->io;
  struct ppu_memory* mem = console->ppu->memory;

  if (addr >= 0x4000) {
    addr = addr % 0x4000;
  }

  if (addr < 0x2000) {
    struct mapper* mapper = (struct mapper*) console->mapper;
    mapper_write(mapper, addr, byte);
  } else if (addr < 0x3f00) {
    uint16_t base = addr < 0x3000 ? addr - 0x2000 : addr - 0x3000;
    uint16_t n = base % PPU_NTAT_SIZE;
    uint8_t *nt, *at;
    switch (base / PPU_NTAT_SIZE) {
      case 0:
        nt = mem->nametable0; at = mem->attrtable0;
        break;
      case 1:
        nt = mem->nametable1; at = mem->attrtable1;
        break;
      case 2:
        nt = mem->nametable2; at = mem->attrtable2;
        break;
      case 3:
        nt = mem->nametable3; at = mem->attrtable3;
        break;
      default: break; /* never gonna happen */
    }

    if (n < PPU_NAMETABLE_SIZE) {
      // nametable
      nt[n] = byte;
    } else {
      // attrtable
      at[n - PPU_NAMETABLE_SIZE] = byte;
    }
  } else if (addr < 0x4000) {
    uint16_t n = addr % (2*PPU_PALETTE_SIZE);
    if (n < 0x0f) {
      mem->image_palette[n] = byte;
    } else {
      mem->sprite_palette[n - 0x0f] = byte;
    }
  }
}
