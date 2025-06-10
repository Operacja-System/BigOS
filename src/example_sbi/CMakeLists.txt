SETUP_EXECUTABLE(example_sbi)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(example_sbi PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(example_sbi
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# DEBUG: Add vfs target for testing
target_link_libraries(example_sbi PRIVATE stdbigos vfs)

COMPILE_BINARY(example_sbi)
ADD_QEMU_TARGET(example_sbi)
