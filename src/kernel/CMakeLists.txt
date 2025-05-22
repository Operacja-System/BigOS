SETUP_EXECUTABLE(kernel)

target_link_libraries(kernel PRIVATE Debug)
target_compile_definitions(kernel PRIVATE __DEBUG__)

COMPILE_BINARY(kernel)
ADD_QEMU_TARGET(kernel BIOS_IMAGE)
