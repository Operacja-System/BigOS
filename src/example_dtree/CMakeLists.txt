SETUP_EXECUTABLE(example_dtree)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(example_dtree PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(example_dtree
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_dtree PRIVATE stdbigos)
target_link_libraries(example_dtree PRIVATE drivers)

COMPILE_BINARY(example_dtree)
ADD_QEMU_TARGET(example_dtree)
