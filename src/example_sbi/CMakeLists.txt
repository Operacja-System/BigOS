SETUP_EXECUTABLE(example_sbi)

target_link_options(example_sbi PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
target_link_libraries(example_sbi PRIVATE stdbigos)

COMPILE_BINARY(example_sbi)
ADD_QEMU_TARGET(example_sbi)
