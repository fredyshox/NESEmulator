//
// Tests ppu background operations
//

extern "C" {
	#include "ppu/renderer.h"
}

#include <gtest/gtest.h>
#include <string>
#include <iostream>

// shift storage

TEST(PPUBackground, ShiftStorageCycle) {
	struct ppu_shift_storage* storage = ppu_shift_storage_create();
	struct ppu_shift_storage* temp = storage;
	int expectedCycleLength = 3;
	int length = 0;
	while (length < expectedCycleLength) {
		ASSERT_TRUE(temp != nullptr);
		length += 1;
		temp = temp->next;
	}

	ASSERT_TRUE(temp == storage);
}

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
