// 
// NES controller interface
//

#ifndef controller_h
#define controller_h

#include <stdbool.h>
#include <stdint.h>

#define CONTROLLER_JOYPAD1 0x4016
#define CONTRoLLER_JOYPAD2 0x4017
#define CONTROLLER_BUTTON_COUNT 8

enum controller_button {
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

typedef struct controller_state controller_state;

struct controller_t {
  uint8_t id;
  bool locked;
  uint32_t counter; 
  struct controller_state state;
  uint8_t buffer;
};

typedef struct controller_t controller_t;

// creation
void controller_init(struct controller_t* cont);
// controller "registers"
void controller_write(struct controller_t* cont, uint8_t byte);
uint8_t controller_read(struct controller_t* cont);

// frontend interface
void controller_set_buttons(struct controller_t* cont, struct controller_state buttons);

#endif /* controller_h */
