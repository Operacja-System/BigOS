SETUP_EXECUTABLE(example_machine)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(example_machine PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(example_machine
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_machine PRIVATE Debug)
target_compile_definitions(example_machine PRIVATE __DEBUG__)

target_link_libraries(example_machine PRIVATE Drivers)

COMPILE_BINARY(example_machine)
ADD_QEMU_TARGET(example_machine BIOS_IMAGE)
