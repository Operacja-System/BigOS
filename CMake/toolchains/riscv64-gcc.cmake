cmake_minimum_required(VERSION 3.13)

if(RISCV_GCC_TOOLCHAIN_INCLUDED)
  return()
endif()

set(RISCV_GCC_TOOLCHAIN_INCLUDED true)

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
