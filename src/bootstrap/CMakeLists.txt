SETUP_EXECUTABLE(kboot)

set(EMBED_PATH $<TARGET_FILE:kernel>)

set(KERNEL_BIN "${CMAKE_CURRENT_BINARY_DIR}/kernel-$<CONFIG>.o")

add_custom_command(
    OUTPUT ${KERNEL_BIN}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ./kernel
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf64-littleriscv kernel ${KERNEL_BIN}
    DEPENDS kernel
    COMMENT "Copying $<TARGET_FILE:kernel> to ${KERNEL_BIN}"
    VERBATIM
)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(kboot PRIVATE -T ${LINKER_SCRIPT} -no-pie)
set_target_properties(kboot
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
target_compile_options(kboot PRIVATE -fno-pie -static) # for now


target_sources(kboot PRIVATE ${KERNEL_BIN})
target_include_directories(kboot PRIVATE ../kernel)
target_link_libraries(kboot PRIVATE stdbigos Debug)

COMPILE_BINARY(kboot)
ADD_QEMU_TARGET(kboot)
