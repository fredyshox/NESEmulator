cmake_minimum_required(VERSION 3.7)

set(COMMON_SOURCES
	utility.c)

add_library(common
	STATIC
	${COMMON_SOURCES})

export_headers(common ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include/common)

target_include_directories(common
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
	
install(TARGETS common DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")