cmake_minimum_required(VERSION 3.7)

set(NES_SOURCES
  console.c
  cartridge.c
  controller.c
  mapper.c
  mappers/mapper0.c
  mappers/mapper1.c
  mappers/mapper2.c
  mappers/mapper3.c)

add_library(nes
  STATIC
  ${NES_SOURCES})

export_headers(nes ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include/nes)

target_include_directories(nes
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

#target_compile_definitions(nes PUBLIC DEBUG=1)

target_link_libraries(nes
  ppu
  6502
  common)

install(TARGETS nes DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")
