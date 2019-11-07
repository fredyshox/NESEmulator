//
// 6502 cpu debugger
//

#define DEBUG 1
#define CYCLES 0
#define STATUS 0
#define MEMORY 0

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nes/console.h"
#include "6502/parser.h"
#include "6502/state.h"

#define MAXSIZE 100

int cycles_total = 0;
cartridge cart;
nes_t console;

void debug_execute_step(nes_t* console, int iters) {
  for (int i = 0; i < iters; i++) {
    if (CYCLES) {
      debug_print("Cycles - %d\n", cycles_total);
    }
    if (STATUS) {
      flags6502 status = console->cpu->status;
      debug_print("Status - N: %d, V: %d, B5: %d, B4: %d, D: %d, I: %d, Z: %d, C: %d\n",
                  status.negative, status.overflow, status.bflag5, status.bflag4,
                  status.decimal, status.int_disable, status.zero, status.carry);
    }

    cycles_total += execute_asm(console->cpu);

    if (MEMORY) {
      // for nestest
      uint8_t err1, err2;
      err1 = memory6502_load(console->cpu->memory, 0x0002);
      err2 = memory6502_load(console->cpu->memory, 0x0003);
      debug_print("Memory - $0002 : %02x, $0003 : %02x\n", err1, err2);
    }
  }
}

void print_usage() {
  fprintf(stderr, "Usage: program [-s,c] <path_to_dot_nes_file>\n");
}

int main(int argc, char** argv) {
  char *testfile;
  int itcount = 0, opt;
  uint32_t config, dconfig = 1;
  while ((opt = getopt(argc, argv, "s:c:")) != -1) {
    switch (opt) {
      case 's':
        itcount = atoi(optarg);
        itcount = itcount > 0 ? itcount : 0;
        break;
      case 'c':
        // config = 0xppppffss
        // p - pc, f - status, s - sp
        config = strtol(optarg, NULL, 16);
        dconfig = 0;
        break;
      default:
        break;
    }
  }

  if (optind < argc) {
    testfile = argv[optind++];
  } else {
    print_usage();
    return 1;
  }

  nes_create(&console);
  if (cartridge_from_file(&cart, testfile) != 0 || nes_load_rom(&console, &cart) != 0) {
    fprintf(stderr, "Error: cartridge error\n");
    return 2;
  }

  // for nestest
  if (dconfig == 0) {
    console.cpu->pc = (uint16_t) (config >> 16) & 0xffff;
    console.cpu->status.byte = (uint8_t) (config >> 8) & 0xff;
    console.cpu->sp = (uint8_t) (config & 0xff);
  }

  if (itcount > 0) {
    debug_execute_step(&console, itcount);
    return 0;
  }

  char input[MAXSIZE], *res, *pos;
  while (1) {
      printf("debugger> ");
      res = fgets(input, MAXSIZE, stdin);
      if (res != NULL) {
        if ((pos = strchr(res, '\n')) != NULL)
          *pos = '\0';
        if (strcmp(res, "step") == 0 || strcmp(res, "s") == 0) {
          debug_execute_step(&console, 1);
        } else if (strcmp(res, "exit") == 0) {
          break;
        } else {
          fprintf(stderr, "Error: undefined command %s\n", res);
        }
      } else {
        fprintf(stderr, "Error: reading stdin\n");
        return 3;
      }
  }

  return 0;
}
