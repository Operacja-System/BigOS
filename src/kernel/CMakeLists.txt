SETUP_EXECUTABLE(kernel)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(kernel PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(kernel
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(kernel PRIVATE Debug)
target_compile_definitions(kernel PRIVATE __DEBUG__)

COMPILE_BINARY(kernel)
ADD_QEMU_TARGET(kernel BIOS_IMAGE)
