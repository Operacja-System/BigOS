if(RISCV_CLANG_TOOLCHAIN_INCLUDED)
  return()
endif()
set(RISCV_CLANG_TOOLCHAIN_INCLUDED TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    list(APPEND CMAKE_PREFIX_PATH "${LLVM_PREFIX}")
endif()

set(CMAKE_C_USING_LINKER_LLD "-fuse-ld=lld")
set(CMAKE_ASM_USING_LINKER_LLD "-fuse-ld=lld")

set(CMAKE_LINKER_TYPE LLD)
find_program(CMAKE_C_COMPILER clang REQUIRED)
find_program(CMAKE_ASM_COMPILER clang REQUIRED)

find_program(CMAKE_OBJCOPY NAMES riscv64-linux-gnu-objcopy riscv64-unknown-linux-gnu-objcopy riscv64-unknown-elf-objcopy riscv64-elf-objcopy riscv-none-elf-objcopy REQUIRED)
