// 
// Tests iNES file parsing
//

extern "C" {
	#include "nes/cartridge.h"
}

#include <gtest/gtest.h>
#include <string>
#include <iostream>

using namespace std;

const string romDir = "../Roms/";

TEST(CartridgeTest, iNES1Parsing1) {
    string path = romDir + "cartridge_test1.nes";
    cartridge c;
    int res = cartridge_from_file(&c, (char*) path.c_str());
    ASSERT_EQ(0, res);
    EXPECT_EQ(16384, c.prg_rom_size);
    EXPECT_EQ(16384, c.chr_rom_size);
    EXPECT_EQ(12, c.mapper);
    EXPECT_EQ(TV_SYSTEM_NTSC, c.tv_system);
    EXPECT_EQ(HORIZONTAL, c.mirroring_type);
    // free cartridge
    cartridge_free(&c);
}

TEST(CartridgeTest, iNES1Parsing2) {
    string path = romDir + "cartridge_test2.nes";
    cartridge c;
    int res = cartridge_from_file(&c, (char*) path.c_str());
    ASSERT_EQ(0, res); 
    EXPECT_EQ(32768, c.prg_rom_size);
    EXPECT_EQ(32768, c.chr_rom_size);
    EXPECT_EQ(15, c.mapper);
    EXPECT_EQ(TV_SYSTEM_PAL, c.tv_system);
    EXPECT_EQ(FOUR_SCREEN, c.mirroring_type);
    // free cartridge
    cartridge_free(&c);
}

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
