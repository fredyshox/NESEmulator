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
  bool res = true;
  // these are visible on y_coord
  // so we care only about x_coord
  ppu_sprite sprites[8];
  uint8_t pixelbuf[256];
  uint8_t expected;
  // stacked
  sprites[0].x_coord = 8;
  sprites[1].x_coord = 12;
  sprites[2].x_coord = 16;
  // overlap
  sprites[3].x_coord = 32;
  sprites[4].x_coord = 32;
  // next to
  sprites[5].x_coord = 64;
  sprites[6].x_coord = 72;
  // edge
  sprites[7].x_coord = 252;
  // pixelbuf init

  ppu_sprite_pixel_layout(sprites, 8, pixelbuf, 256);

  int i;
  for (i = 0; i < 256 && res; i++) {
    if (i >= 8 && i < 12) {
      expected = 0x01; // 0b00000001
    } else if (i >= 12 && i < 16) {
      expected = 0x03; // 0b00000011
    } else if (i >= 16 && i < 20) {
      expected = 0x06; // 0b00000110
    } else if (i >= 20 && i < 24) {
      expected = 0x04; // 0b00000100
    } else if (i >= 32 && i < 40) {
      expected = 0x18; // 0b00011000
    } else if (i >= 64 && i < 72) {
      expected = 0x20; // 0b00100000
    } else if (i >= 72 && i < 80) {
      expected = 0x40; // 0b01000000
    } else if (i >= 252) {
      expected = 0x80; // 0b10000000
    } else {
      expected = 0x00;
    }
    res = (expected == pixelbuf[i]);
  }
  ASSERT_EQ(256, i, "(1) sprite pixel layout", &res)
  if (!res) {
    fprintf(stderr, "\033[0;31m(1) pixel val: %x\033[0m\n", pixelbuf[i]);
  }

  assert(res);
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
