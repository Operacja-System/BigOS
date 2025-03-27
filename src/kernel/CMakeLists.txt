SETUP_EXECUTABLE(kernel)

target_link_libraries(kernel PRIVATE Debug)
target_link_options(kernel PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
target_compile_definitions(kernel  PRIVATE __DEBUG__)

ADD_QEMU_TARGET(kernel KERNEL)
