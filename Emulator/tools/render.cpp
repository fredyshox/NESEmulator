// Tests ppu rendering
// Output image should have 8-pixel columns for each color in image pallete (16 colors), repreated twice

// FIXME doesnt work with latest changes

#define DEBUG 1

extern "C" {
#include "ppu/ppu.h"
#include "ppu/color.h"
#include "ppu/renderer.h"
}

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

uint8_t pattern[] = {
  // index 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // index 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // index 2
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // index 3
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

uint8_t nametable[] = {
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
};

uint8_t attrtable[] = {
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
  0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

uint8_t palette[] = {
  0x1e, 0x11, 0x12, 0x13, 0x01, 0x15, 0x16, 0x17, 0x18, 0x0c, 0x1a, 0x1b, 0x1c, 0x14, 0x19, 0x30
};

uint8_t custom_load_handler(struct ppu_memory* mem, uint16_t addr) {
  if (addr < 0x2000) {
    uint8_t* ptable = (uint8_t*) mem->io;
    return ptable[addr % 0x1000];
  }
  fprintf(stderr, "WTF read at %04x\n", addr);
  return 0;
}

void custom_store_handler(struct ppu_memory* mem, uint16_t addr, uint8_t byte) {
  fprintf(stderr, "WTF write at %04x\n", addr);
}

int main(int argc, char** argv) {
  struct ppu_memory mem;
  struct ppu_state ppu;
  struct ppu_render_handle handle = *ppu_render_handle_create();
  uint8_t* bg_ptable = (uint8_t*) malloc(sizeof(uint8_t) * 0xfff);
  memcpy(bg_ptable, pattern, 64);

  ppu_memory_create(&mem);
  ppu_state_create(&ppu, &mem);

  memcpy(mem.image_palette, palette, PPU_PALETTE_SIZE);
  memcpy(mem.nt_buf, nametable, PPU_NAMETABLE_SIZE);
  memcpy((mem.nt_buf + PPU_NAMETABLE_SIZE), attrtable, PPU_ATTRTABLE_SIZE);
  ppu_memory_set_mirroring(&mem, PPU_MIRRORING_SINGLE_SCREEN);
  mem.load_handler = custom_load_handler;
  mem.store_handler = custom_store_handler;
  mem.io = (void*) bg_ptable;

  //ppu.control.nametable_addr = 0;
  ppu.control.bg_pttrntable = 0;

  for (int i = 0; i < VERTICAL_RES; i++) {
    for (int j = 0; j < HORIZONTAL_RES; j++) {
      ppu_execute_cycle(&ppu, &handle);
    }
  }

  if (argc != 2) {
    fprintf(stderr, "Usage: program <path_to_output_image.png>\n");
    return 1;
  }

  uint8_t* frame = handle.frame;
  Mat rgbFrame(VERTICAL_RES, HORIZONTAL_RES, CV_8UC3, Scalar(0, 0, 0));
  for (int i = 0; i < VERTICAL_RES; i++) {
    for (int j = 0; j < HORIZONTAL_RES; j++) {
      Vec3b& color = rgbFrame.at<Vec3b>(i, j);
      color[0] = frame[i * HORIZONTAL_RES * 3 + j * 3];
      color[1] = frame[i * HORIZONTAL_RES * 3 + j * 3 + 1];
      color[2] = frame[i * HORIZONTAL_RES * 3 + j * 3 + 2];
    }
  }

  vector<int> params;
  params.push_back(IMWRITE_PNG_COMPRESSION);
  params.push_back(9);

  try {
    imwrite(argv[1], rgbFrame, params);
  } catch (runtime_error& ex) {
    fprintf(stderr, "Exception writing image: %s\n", ex.what());
    return 2;
  }

  fprintf(stderr, "DONE\n");
  return 0;
}
