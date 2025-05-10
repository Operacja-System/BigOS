project(llvm-libc)

add_library(llvm-libc-c STATIC IMPORTED GLOBAL)
add_library(llvm-libc-m STATIC IMPORTED GLOBAL)

set(LOCATION ${PROJECT_SOURCE_DIR}/lib/riscv64-unknown-none-elf)

set_property(TARGET llvm-libc-c PROPERTY IMPORTED_LOCATION ${LOCATION}/libc.a)
set_property(TARGET llvm-libc-m PROPERTY IMPORTED_LOCATION ${LOCATION}/libm.a)

target_include_directories(llvm-libc-c SYSTEM INTERFACE include)
target_include_directories(llvm-libc-m SYSTEM INTERFACE include)

add_library(libc::libc ALIAS llvm-libc-c)
add_library(libc::libm ALIAS llvm-libc-m)
