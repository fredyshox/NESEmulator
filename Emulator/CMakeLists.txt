cmake_minimum_required(VERSION 3.7)

project(nes-emulator)

add_compile_options(${CMAKE_C_FLAGS} -std=c11)
add_compile_options(${CMAKE_C_FLAGS} -Wall)
add_compile_options(${CMAKE_C_FLAGS} -pedantic)
add_compile_options(${CMAKE_C_FLAGS} -Wno-gnu-binary-literal)

add_subdirectory(6502lib)
add_subdirectory(ppu)

enable_testing()
add_subdirectory(test)
