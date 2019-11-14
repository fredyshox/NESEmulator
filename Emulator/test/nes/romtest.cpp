//
// Tests using various emulator test roms
//

extern "C" {
	#include "nes/console.h"
}

#include <gtest/gtest.h>
#include <string>
#include <iostream>

using namespace std;

#define MAX_INSTRUCTIONS 5000000
const string basepath = "../testsuite/roms/";

class RomTest : public ::testing::Test {
protected:
	const string basepath = "../testsuite/roms/";
	nes_t console;
	cartridge cart;

	// Ticks emulator and checks memory location at address.
	// Waits for change predecessor -> expected which signals test result.
	void testRom(string& path, uint16_t address, uint8_t expected, uint8_t predecessor, int max_instructions) {
		if (cartridge_from_file(&cart, (char*) path.c_str()) != 0 || nes_load_rom(&console, &cart) != 0) {
			cerr << "Error: rom related" << endl;
			FAIL();
		}

		int counter = 0;
		bool predecessorFound = false;
		uint8_t result;
		memory6502* memory = console.cpu->memory;
		int error;
		while (true) {
			ASSERT_GT(max_instructions, counter);
			nes_step(&console, &error);
			ASSERT_EQ(error, 0);

			result = memory6502_load(memory, address);
			if (predecessorFound && result != predecessor) {
				break;
			} else if (!predecessorFound && result == predecessor) {
				predecessorFound = true;
				cerr << "Found predecessor" << endl;
			}

			counter += 1;		
		}

		cerr << "Result code: " << (unsigned int) result << endl;
		ASSERT_EQ(result, expected);
	}

	// setup/teardown
	void SetUp() override {
		nes_create(&console);
	}	

	void TearDown() override {
		nes_reset(&console);
		cartridge_free(&cart);
	}
};

// CPU

// TODO MAPPER 1 REQURED
// TEST_F(RomTest, OfficialAsm) {
// 	string path = basepath + "instr_test-v5/official_only.nes";
// 	test_rom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
// }

// TEST_F(RomTest, BranchTiming) {
// 	string path1 = basepath + "branch_timing_tests/1.Branch_Basics.nes";
// 	string path2 = basepath + "branch_timing_tests/2.Backward_Branch.nes";	
// 	string path3 = basepath + "branch_timing_tests/3.Forward_Branch.nes";
// 	cerr << path1 << endl;	
// 	testRom(path1, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
// 	cerr << path2 << endl;
// 	testRom(path2, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
// 	cerr << path3 << endl;
// 	testRom(path3, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
// }

// PPU

TEST_F(RomTest, OAMRead) {
	string path = basepath + "oam_read/oam_read.nes"; 
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

TEST_F(RomTest, OAMStress) {
	string path = basepath + "oam_stress/oam_stress.nes";
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

TEST_F(RomTest, SpriteZeroHit) {
	string path = basepath + "ppu_sprite_hit/ppu_sprite_hit.nes";
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

TEST_F(RomTest, SpriteOverflow) {
	string path = basepath + "ppu_sprite_overflow/ppu_sprite_overflow.nes";
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

TEST_F(RomTest, VBlankAndNMI) {
	string path = basepath + "ppu_vbl_nmi/ppu_vbl_nmi.nes";
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);	
}

TEST_F(RomTest, OpenBus) {
	string path = basepath + "ppu_open_bus/ppu_open_bus.nes";
	testRom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
