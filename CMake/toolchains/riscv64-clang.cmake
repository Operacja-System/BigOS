cmake_minimum_required(VERSION 3.13)

if(RISCV_CLANG_TOOLCHAIN_INCLUDED)
  return()
endif()
set(RISCV_CLANG_TOOLCHAIN_INCLUDED TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

set(CMAKE_C_COMPILER clang)
set(CMAKE_OBJCOPY riscv64-linux-gnu-objcopy)
