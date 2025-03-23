SETUP_EXECUTABLE(example)

target_link_libraries(example PRIVATE Debug)
target_link_options(example PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
target_compile_definitions(example PRIVATE __DEBUG__)

COMPILE_BINARY(example)
ADD_QEMU_TARGET(example BIOS_IMAGE)
