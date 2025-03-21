SETUP_EXECUTABLE(example_machine)

target_link_libraries(example_machine PRIVATE Debug)
target_link_options(example_machine PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
target_compile_definitions(example_machine PRIVATE __DEBUG__)

COMPILE_BINARY(example_machine)
ADD_QEMU_TARGET(example_machine BIOS_IMAGE)
