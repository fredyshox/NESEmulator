

#include <stdint.h>
#include <stdio.h>


struct mem_addr {
  int type;
  union {
    uint16_t value;
    uint8_t lval;
    uint8_t hval;
  };
};

int main() {
  uint16_t x = 0x01f0;
  uint8_t u = (uint8_t) x;
  printf("%u\n", u);
  int a = 1;
  if (a == 1)    printf("sss"); 
  return 0;
}
