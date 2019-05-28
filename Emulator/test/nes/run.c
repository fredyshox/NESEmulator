#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "nes/console.h"
#include "6502/parser.h"
#include "6502/state.h"

cartridge cart;
nes_t console;
double lastTime;

double getTimeUSec() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return ((double) time.tv_sec * 1E6 + (double) time.tv_usec);
}

void emulate() {
  while (1) {
            double current = getTimeUSec();
            if (lastTime == 0.0) {
                lastTime = current;
            }

            if (current != lastTime) {
                nes_step_time(&console, current - lastTime);
            }

            lastTime = current;
            usleep(5000);
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "KUPA\n");
    return 1;
  }

  char *testfile = argv[1];
  nes_create(&console);
  if (cartridge_from_file(&cart, testfile) != 0 || nes_load_rom(&console, &cart) != 0) {
    fprintf(stderr, "Error: cartridge error\n");
    return 2;
  }
  emulate();

  return 0;
}
