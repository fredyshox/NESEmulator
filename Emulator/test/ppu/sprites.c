//
// sprites.c
// Tests sprite related ppu functions: evaluation, layout, sprite attributes, priorities etc.
//

#include "pputest.h"

void test_sprite_eval() {
  bool res = true;
  ppu_state ppu;
  ppu_sprite sprites[64];
  ppu_sprite sprbuf[8];
  uint8_t y_coord = 64;
  int spr_count;
  ppu.sprite_ram = sprites;

  // 6 sprites on y_coord
  for (int i = 10; i < 64; i++) {
    if (i % 10 == 0) {
      sprites[i].index = i / 10;
      sprites[i].y_coord = (uint8_t) y_coord;
    } else {
      sprites[i].y_coord = 0;
    }
  }

  spr_count = ppu_evaluate_sprites(&ppu, sprbuf, 8, y_coord);
  ASSERT_EQ_DO(6, spr_count, "(1) Sprite eval (6 sprites)", &res) {
    bool count_check = true;
    for (int j = 0; j < spr_count; j++) {
      count_check = sprbuf[j].index == j + 1 &&
                    sprbuf[j].y_coord <= y_coord &&
                    sprbuf[j].y_coord + TILE_SIZE > y_coord;
    }
    ASSERT_T(count_check, "(1) (a) sprite eval data", &res);
  }
  if (!res) goto test_sprite_eval_end;

  // 9 sprites on y_coord
  y_coord = 128;
  for (int i = 0; i < 64; i ++) {
    if (i % 7 == 0) {
      sprites[i].index = i / 7;
      sprites[i].y_coord = (uint8_t) y_coord;
    } else {
      sprites[i].y_coord = 0;
    }
  }

  // sprbuffer count must be 8 which is max
  spr_count = ppu_evaluate_sprites(&ppu, sprbuf, 8, y_coord);
  ASSERT_EQ_DO(8, spr_count, "(2) Sprite eval (10 sprites)", &res) {
    bool count_check = true;
    for (int j = 0; j < spr_count; j++) {
      count_check = sprbuf[j].index == j &&
                    sprbuf[j].y_coord >= y_coord &&
                    sprbuf[j].y_coord < y_coord + TILE_SIZE;
    }
    ASSERT_T(count_check, "(2) (a) sprite eval data", &res);
  }
  if (!res) goto test_sprite_eval_end;
  // sprite overflow
  ASSERT_EQ(1, ppu.status.sprite_ovf, "(2) (b) sprite overflow", &res);

  test_sprite_eval_end:
    assert(res);
}

void test_sprite_pixel_layout() {

}

int main(int argc, char** argv) {
  if (argc != 2)
    return 1;

  if (strcmp(argv[1], "--eval") == 0) {
    test_sprite_eval();
  } else if (strcmp(argv[1], "--layout") == 0) {
    test_sprite_pixel_layout();
  }

  return 0;
}
