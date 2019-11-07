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

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
