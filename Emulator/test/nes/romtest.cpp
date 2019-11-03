//
// Tests ppu memory
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
	void test_rom(string& path, uint16_t address, uint8_t expected, uint8_t predecessor, int max_instructions) {
		if (cartridge_from_file(&cart, (char*) path.c_str()) != 0 || nes_load_rom(&console, &cart)) {
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

TEST_F(RomTest, OAMRead) {
	string path = basepath + "oam_read/oam_read.nes"; 
	test_rom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

TEST_F(RomTest, OAMStress) {
	string path = basepath + "oam_stress/oam_stress.nes";
	test_rom(path, 0x6000, 0x00, 0x80, MAX_INSTRUCTIONS);
}

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
