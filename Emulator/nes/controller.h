// 
// NES controller interface
//

#ifndef controller_h
#define controller_h

#include <stdbool.h>
#include <stdint.h>

#define CONTROLLER_JOYPAD1 0x4016
#define CONTRoLLER_JOYPAD2 0x4017

enum controller_buttons {
  A_BUTTON = 0,
  B_BUTTON,
  SELECT_BUTTON,
  START_BUTTON,
  UP_BUTTON,
  DOWN_BUTTON,
  LEFT_BUTTON,
  RIGHT_BUTTON
};

struct controller_state {
  bool buttons[8];
};

struct controller_t {
  uint8_t id;
  bool locked;
  uint32_t counter; 
  uint8_t state;
};

typedef struct controller_t controller_t;

// controller "registers"
void controller_write(struct controller_t* cont, uint8_t byte);
uint8_t controller_read(struct controller_t* cont);

// frontend interface
void controller_set_buttons(struct controller_t* cont, struct controller_state buttons);

#endif /* controller_h */
