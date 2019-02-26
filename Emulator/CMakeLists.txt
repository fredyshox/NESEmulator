cmake_minimum_required(VERSION 3.7)

project(nes-emulator)

add_compile_options(${CMAKE_C_FLAGS} -Wall)
add_compile_options(${CMAKE_C_FLAGS} -pedantic)

add_subdirectory(6502lib)

enable_testing()
add_subdirectory(test)
