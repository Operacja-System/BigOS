cmake_minimum_required(VERSION 3.24)

find_program(QEMU_FOUND_PATH "qemu-system-riscv64")

set(BIGOS_WARNINGS_AS_ERRORS OFF CACHE BOOL "Treat warnings as errors")
set(BIGOS_QEMU_PATH "${QEMU_FOUND_PATH}" CACHE PATH "path to qemu")
set(BIGOS_QEMU_OPTIONS "-machine virt -serial mon:stdio -nographic" CACHE STRING "options for qemu")

separate_arguments(BIGOS_QEMU_OPTIONS_LIST UNIX_COMMAND "${BIGOS_QEMU_OPTIONS}")

function(SETUP_COMMON name)
    target_compile_features( ${name} PUBLIC c_std_23 )
    set_target_properties( ${name} PROPERTIES
        VISIBILITY_INLINES_HIDDEN true
        C_EXTENSIONS ON
        COMPILE_WARNING_AS_ERROR ${BIGOS_WARNINGS_AS_ERRORS}
    )

    target_compile_options(${name} PRIVATE -Wall -Wextra -Wno-ignored-qualifiers -Wno-unknown-pragmas)
endfunction()

function(SETUP_LIBRARY name)
    add_library(${name})
    SETUP_COMMON(${name})

    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.c)
    file(GLOB_RECURSE AS_SOURCES CONFIGURE_DEPENDS *.s)
    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/${name}/*.h")

    target_sources(${name}
      PRIVATE
        ${SOURCES}
        ${AS_SOURCES}
      PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${PROJECT_SOURCE_DIR}/include"
        FILES ${HEADERS}
      )
endfunction()

function(SETUP_EXECUTABLE name)
    add_executable(${name})
    SETUP_COMMON(${name})

    file(GLOB SOURCES CONFIGURE_DEPENDS *.c)
    file(GLOB HEADERS CONFIGURE_DEPENDS *.h)
    file(GLOB AS_SOURCES CONFIGURE_DEPENDS *.s)

    target_sources(${name}
      PRIVATE
        ${SOURCES}
        ${AS_SOURCES}
      PRIVATE
        FILE_SET HEADERS
        BASE_DIRS .
        FILES ${HEADERS}
      )
endfunction()

function(COMPILE_BINARY name)
    add_custom_command(
        TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY}
                    -O binary
                    "$<TARGET_FILE:${name}>"
                    "$<TARGET_FILE:${name}>.bin"
        VERBATIM
    )

    install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${name}.bin" TYPE BIN)
endfunction()

function(ADD_QEMU_TARGET name)
    cmake_parse_arguments(
        arg
        "BIOS_IMAGE"
        ""
        ""
        ${ARGN}
    )

    set(CMD "${BIGOS_QEMU_PATH}" ${BIGOS_QEMU_OPTIONS_LIST}
            $<IF:$<BOOL:${arg_BIOS_IMAGE}>,-bios,-kernel>
            "$<TARGET_FILE:${name}>.bin"
        )

    add_custom_target(run-${name}
        COMMAND ${CMD}
        DEPENDS ${name}
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(debug-${name}
        COMMAND ${CMD} -S -s
        DEPENDS ${name}
        VERBATIM
        USES_TERMINAL
    )
endfunction()
