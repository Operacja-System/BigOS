cmake_minimum_required(VERSION 3.13)

if(RISCV_GCC_TOOLCHAIN_INCLUDED)
    return()
endif()

set(RISCV_GCC_TOOLCHAIN_INCLUDED true)

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

foreach(PREFIX "riscv64-unknown-linux-elf" "riscv64-linux-gnu" "riscv64-unknown-linux-gnu")
    find_program(PREFIX_TOOLCHAIN_GCC "${PREFIX}-gcc")
    if (PREFIX_TOOLCHAIN_GCC)
        set(DEFAULT_RISCV_TOOLCHAIN_PREFIX "${PREFIX}-")
        break()
    endif()
endforeach()

set(RISCV_TOOLCHAIN_PREFIX "${DEFAULT_RISCV_TOOLCHAIN_PREFIX}" CACHE STRING "RISC-V toolchain prefix")

if (NOT RISCV_TOOLCHAIN_PREFIX)
    message(SEND_ERROR "Toolchain prefix is not specified")
endif()

find_program(CMAKE_C_COMPILER "${RISCV_TOOLCHAIN_PREFIX}gcc" REQUIRED)
