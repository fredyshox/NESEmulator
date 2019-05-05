cmake_minimum_required(VERSION 3.7)

set(PPU_SOURCES
  color.c
  ppu.c
  renderer.c)

add_library(ppu
  STATIC
  ${PPU_SOURCES})

export_headers(ppu ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include/ppu)

target_include_directories(ppu
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

install(TARGETS ppu DESTINATION lib)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")
