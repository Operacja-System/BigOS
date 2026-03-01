SETUP_EXECUTABLE(example_machine)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(example_machine PRIVATE -static-pie -T ${LINKER_SCRIPT})
set_target_properties(example_machine
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_machine PRIVATE Debug trap relocations)

ADD_QEMU_TARGET(example_machine BIOS_IMAGE)
