//
// Test op which manipulate flags (compares etc.)
// Copyright (c) 2019 Kacper Rączy
//

#include "test.h"

//TODO test flag operation and compares

void test_flags() {
  // TODO
}

void test_compares() {
  // TODO
}

int main(int argc, char** argv) {
  if (argc != 2)
    return 1;

  if (strcmp(argv[1], "--flag") == 0) {
    test_flags();
  } else if (strcmp(argv[1], "--cmp") == 0) {
    test_compares();
  }

  return 0;
}
