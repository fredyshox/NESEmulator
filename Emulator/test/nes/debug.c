//
// 6502 cpu debugger
//

#define DEBUG 1
#define CYCLES 0

#include <stdio.h>
#include <string.h>
#include "nes/console.h"
#include "6502/parser.h"
#include "6502/state.h"

#define MAXSIZE 100

int cycles_total = 0;
cartridge cart;
nes_t console;

void debug_execute_step(nes_t* console, int iters) {
  uint8_t err1, err2;
  cycles_total += execute_asm(console->cpu);
  if (CYCLES)
    debug_print("Cycles: %d\n", cycles_total);
  // for nestest
  err1 = memory6502_load(console->cpu->memory, 0x0002);
  err2 = memory6502_load(console->cpu->memory, 0x0003);
  debug_print("Memory: $0002 : %02x, $0003 : %02x\n", err1, err2);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: program <path_to_dot_nes_file>\n");
    return 1;
  }

  char* testfile = argv[1];
  char input[MAXSIZE], *res, *pos;
  nes_create(&console);

  if (cartridge_from_file(&cart, testfile) != 0 || nes_load_rom(&console, &cart) != 0) {
    fprintf(stderr, "Error: cartridge error\n");
    return 2;
  }

  // for nestest
  console.cpu->pc = 0xc000;

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
