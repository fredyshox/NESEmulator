// 
// NES controller interface
//

#include "nes/controller.h"

uint8_t controller_state_to_byte(struct controller_state state) {
  uint8_t res = 0;
  for (int i = 0; i < 8; i++) {
    if (state.buttons[i]) {
      res |= (0x01 << i);
    }
  }

  return res;
}

void controller_write(struct controller_t* cont, uint8_t byte) {
  cont->locked = !(byte & 0x01);
  if (cont->locked) {
    cont->counter = 0;
    cont->buffer = controller_state_to_byte(cont->state);
  }
}

uint8_t controller_read(struct controller_t* cont) {
  if (cont->counter >= 8) {
    // official controllers return 1 after 8 reads
    return 0x01;
  }

  uint8_t byte = (cont->buffer & 0x01);
  cont->buffer >>= 1;
  cont->counter += 1;

  return byte;
}

void controller_set_buttons(struct controller_t* cont, struct controller_state buttons) {
  cont->state = buttons;
}
