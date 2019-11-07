//
// Tests ppu memory
//

extern "C" {
	#include "ppu/ppu.h"
}

#include <gtest/gtest.h>
#include <string>
#include <iostream>

int main(int argc, char** argv) {
 	::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
