//
// Tests ppu sprite operations
//

extern "C" {
	#include "ppu/ppu.h"
	#include "ppu/renderer.h"
}

#include <gtest/gtest.h>
#include <string>
#include <iostream>

using namespace std;

class PPUSprites: public ::testing::Test {
protected:
	ppu_state ppu;
	ppu_memory mem;
	ppu_sprite sprites[64];
	uint8_t pttrntable[0x1000];

	void spriteTestData(uint8_t yCoord, int count) {
		int step = 64 / count;
		for (int i = 0; i < 64; i++) {
			if (i % step == 0 && count > 0) {
				sprites[i].index = (i / step);
				sprites[i].y_coord = yCoord;
				count -= 1;
			} else {
				sprites[i].index = i + step;
				sprites[i].y_coord = yCoord + TILE_SIZE + 1;
			}
			sprites[i].palette_msb = 0x00;
		}
	}

	void testSpriteEvaluationNoOvf(uint8_t yCoord, int count) {
		assert(count <= 8);

		spriteTestData(yCoord, count);
		ppu_sprite sprbuf[8];
		int sprCount = ppu_evaluate_sprites(&ppu, sprbuf, 8, yCoord);
		ASSERT_EQ(count, sprCount);
		ASSERT_EQ(0, ppu.status.sprite_ovf);
		for (int i = 0; i < count; i++) {
			bool temp = sprbuf[i].index == i && 
						sprbuf[i].y_coord <= yCoord && 
						sprbuf[i].y_coord + TILE_SIZE > yCoord;
			EXPECT_TRUE(temp);
		}	
	}

	static uint8_t memoryLoadHandler(ppu_memory* mem, uint16_t address) {
		uint8_t* pttrntable = (uint8_t*) mem->io;
		return pttrntable[address & 0xfff];
	}

	void SetUp() override {
		ppu_state_create(&ppu, &mem);
		mem.sprite_ram = sprites;
		memset(pttrntable, 0, 0x1000);
		mem.io = pttrntable;
		mem.load_handler = PPUSprites::memoryLoadHandler;
	}

	void TearDown() override {}
};

TEST_F(PPUSprites, SpriteEvaluation) {
	testSpriteEvaluationNoOvf(64, 6);
	testSpriteEvaluationNoOvf(128, 8);
}

TEST_F(PPUSprites, SpriteOverflow) {
	uint8_t yCoord = 128;
	int count = 9;
	spriteTestData(yCoord, count);
	ppu_sprite sprbuf[8];
	int sprCount = ppu_evaluate_sprites(&ppu, sprbuf, 8, yCoord);
	ASSERT_EQ(8, sprCount);
	ASSERT_EQ(1, ppu.status.sprite_ovf);
}

TEST_F(PPUSprites, SpritePixelLayout) {
	spriteTestData(128, 8);
	ppu_sprite sprbuf[8];
	ppu_evaluate_sprites(&ppu, sprbuf, 8, 128);
	// xcoords
	//// stacked
	sprbuf[0].x_coord = 8;
	sprbuf[1].x_coord = 12;
	sprbuf[2].x_coord = 16;
	//// overlap
	sprbuf[3].x_coord = 32;
	sprbuf[4].x_coord = 32;
	//// next to
	sprbuf[5].x_coord = 64;
	sprbuf[6].x_coord = 72;
	//// edge
	sprbuf[7].x_coord = 252;

	// patterns
	// 0000 0303 3131 3333
	pttrntable[sprbuf[0].index * 16] = 0x00;
	pttrntable[sprbuf[0].index * 16 + TILE_SIZE] = 0x00;
	pttrntable[sprbuf[1].index * 16] = 0x55;
	pttrntable[sprbuf[1].index * 16 + TILE_SIZE] = 0x00;	
	pttrntable[sprbuf[2].index * 16] = 0xff;
	pttrntable[sprbuf[2].index * 16 + TILE_SIZE] = 0xff;
	// 1111 1111 (not 3333 3333)
	pttrntable[sprbuf[3].index * 16] = 0xff;
	pttrntable[sprbuf[3].index * 16 + TILE_SIZE] = 0x00;
	pttrntable[sprbuf[4].index * 16] = 0xff;
	pttrntable[sprbuf[4].index * 16 + TILE_SIZE] = 0xff;
	// 2222 2222 1111 1111
	pttrntable[sprbuf[5].index * 16] = 0x00;
	pttrntable[sprbuf[5].index * 16 + TILE_SIZE] = 0xff;
	pttrntable[sprbuf[6].index * 16] = 0xff;
	pttrntable[sprbuf[6].index * 16 + TILE_SIZE] = 0x00;
	// 3333
	pttrntable[sprbuf[7].index * 16] = 0xff;
	pttrntable[sprbuf[7].index * 16 + TILE_SIZE] = 0xff;

	uint8_t buffer[HORIZONTAL_RES];
	uint8_t expectedBuffer[HORIZONTAL_RES];
	ppu.fine_y = 0;
	ppu.control.spr_pttrntable = 0;
	ppu_sprite_pixel_layout(&ppu, sprbuf, 8, buffer, HORIZONTAL_RES, 128);
	memset(expectedBuffer, 0, HORIZONTAL_RES);
	memset((expectedBuffer + 20), 3, 4);
	memset((expectedBuffer + 32), 1, 8);
	memset((expectedBuffer + 64), 2, 8);
	memset((expectedBuffer + 72), 1, 8);
	memset((expectedBuffer + 252), 3, 4);

	uint8_t val;
	for (int i = 0; i < HORIZONTAL_RES; i++) {
		cout << "Testing index: " << i << endl;
		val = buffer[i] & 0x0f;
		if (i >= 12 && i < 16) {
			if (i % 2 == 0)
				EXPECT_EQ(0, val);
			else 
				EXPECT_EQ(1, val);
		} else if (i >= 16 && i < 20) {
			if (i % 2 == 0)
				EXPECT_EQ(3, val);
			else 
				EXPECT_EQ(1, val);	
		} else {
			EXPECT_EQ(expectedBuffer[i], val);
		}
	}
}

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}
