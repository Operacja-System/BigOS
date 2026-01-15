SETUP_EXECUTABLE(example_dtree)

target_include_directories(example_dtree PRIVATE ${CMAKE_SOURCE_DIR}/src/kernel)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# If there's no -static then uart doesn't work with riscv64-linux-gnu
target_link_options(example_dtree PRIVATE -static -T ${LINKER_SCRIPT})
set_target_properties(example_dtree
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_dtree PRIVATE stdbigos dt Debug)

COMPILE_BINARY(example_dtree)
ADD_QEMU_TARGET(example_dtree)
