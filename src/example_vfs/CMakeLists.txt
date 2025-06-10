SETUP_EXECUTABLE(example_vfs)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(example_vfs PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(example_vfs
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_vfs PRIVATE stdbigos vfs)

COMPILE_BINARY(example_vfs)
ADD_QEMU_TARGET(example_vfs)
