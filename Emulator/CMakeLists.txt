cmake_minimum_required(VERSION 3.7)

function(export_headers TARGET HEADER_SOURCE_DIR HEADER_DEST_DIR)

    # Put all headers that are in the source directory into EXPORT_HEADERS variable
    file(GLOB_RECURSE EXPORT_HEADERS CONFIGURE_DEPENDS
        RELATIVE "${HEADER_SOURCE_DIR}"
        "${HEADER_SOURCE_DIR}/*.h"
    )

    # For each header that will be exported
    foreach(HEADER ${EXPORT_HEADERS})

        # Get the directory portion that needs to be created
        get_filename_component(HEADER_DIRECTORY "${HEADER}" DIRECTORY)

        # Create the directory
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${HEADER_DEST_DIR}/${HEADER_DIRECTORY}"
        )

        if (MSVC)

            # Make a hard link to the file
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND if not exist "${HEADER_DEST_DIR}/${HEADER}" \( mklink /h "${HEADER_DEST_DIR}/${HEADER}" "${HEADER_SOURCE_DIR}/${HEADER}" \)
            )

        else()

            # Make a symbolic link to the file
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ln -sf "${HEADER_SOURCE_DIR}/${HEADER}" "${HEADER_DEST_DIR}/${HEADER}"
            )

        endif()

    endforeach(HEADER)

endfunction()

project(nes-emulator)

add_compile_options(${CMAKE_C_FLAGS} -std=c11)
add_compile_options(${CMAKE_C_FLAGS} -Wall)
add_compile_options(${CMAKE_C_FLAGS} -pedantic)
add_compile_options(${CMAKE_C_FLAGS} -Wno-gnu-binary-literal)
add_compile_options(${CMAKE_C_FLAGS} -Wno-gnu-anonymous-struct)
add_compile_options(${CMAKE_C_FLAGS} -Wno-nested-anon-types)
add_compile_options(${CMAKE_C_FLAGS} -Wno-gnu-zero-variadic-macro-arguments)

add_subdirectory(common)
add_subdirectory(ppu)
add_subdirectory(6502)
add_subdirectory(nes)

enable_testing()
add_subdirectory(test)
