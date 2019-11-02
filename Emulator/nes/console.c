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
  int c, p;
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
  debug_print("ROM reset vector: %04x\n", console->cpu->pc);
  // init ppu (set nametable addr)
  mem = console->ppu->memory;
  ppu_memory_set_mirroring(mem, cartridge->mirroring_type);

  return m;
}

bool nes_is_loaded(nes_t* console) {
  return (console->cartridge != NULL && console->mapper != NULL);
}

void nes_reset(nes_t* console) {
  if (!nes_is_loaded(console)) {
    return;
  }

  mapper* mapper = console->mapper;
  console->mapper = NULL;
  console->cartridge = NULL;
  mapper_free(mapper);
  // reset ppu and cpu
  state6502_create(console->cpu, console->cpu->memory);
  ppu_state_create(console->ppu, console->ppu->memory);
}

int nes_step(nes_t* console, int* error) {
  int cpu_cycles;
  cpu_cycles = execute_asm(console->cpu);
  if (cpu_cycles <= 0) {
    error = 1;
    return 0;
  }

  for (int i = 0; i < cpu_cycles * 3; i++) {
    ppu_execute_cycle(console->ppu, console->ppu_handle);
    // TODO do sth between vblank and new frame
    if (console->ppu_handle->nmi_trigger && console->ppu->control.gen_nmi) {
      console->cpu->incoming_int = NMI_INT;
      console->ppu_handle->nmi_trigger = false;
    }
  }

  *error = 0;
  return cpu_cycles;
}

void nes_step_time(nes_t* console, double seconds) {
  long long int cpu_cycles = (long long int)(NES_CPU_FREQ * seconds);
  int nes_error;
  while (cpu_cycles > 0) {
    cpu_cycles -= nes_step(console, &nes_error);
    if (nes_error) {
      pretty_print(stdout, "NES ERROR: CPU error code: %d", nes_error);
      break; //TODO propagate error message
    }
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
        debug_print("Warning: CPU read access on %d\n", addr);
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
        debug_print("Warning: CPU write access on %d\n", addr);
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
        debug_print("Warning: Not implemented - write on %d\n", addr);
        break;
    }
  } else {
    mapper_write(console->mapper, addr, byte);
  }
}

uint8_t nes_ppu_memory_read(struct ppu_memory* memory, uint16_t addr) {
  nes_t* console = (nes_t*) memory->io;

  if (addr < 0x2000) {
    struct mapper* mapper = (struct mapper*) console->mapper;
    uint8_t out;
    mapper_read(mapper, addr, &out);
    return out;
  }

  return 0; // should never happen
}

void nes_ppu_memory_write(struct ppu_memory* memory, uint16_t addr, uint8_t byte) {
  nes_t* console = (nes_t*) memory->io;

  if (addr < 0x2000) {
    struct mapper* mapper = (struct mapper*) console->mapper;
    mapper_write(mapper, addr, byte);
  }
}
