/**
* C playground to test some features.
 */

#include <stdint.h>
#include <stdio.h>

struct flags6502 {
  uint8_t negative : 1;
  uint8_t overflow : 1;
  uint8_t unused : 1;
  uint8_t brk : 1;
  uint8_t decimal : 1;
  uint8_t interrupt : 1;
  uint8_t zero : 1;
  uint8_t carry : 1;
};

struct mem_addr {
  int type;
  union {
    uint16_t value;
    uint8_t lval;
    uint8_t hval;
  };
};

int main() {
  struct flags6502 flags;
  memset(&flags, 0, 1);
  uint8_t value = *((uint8_t*) &flags);
  printf("size of %u\n", value);
  return 0;
}
