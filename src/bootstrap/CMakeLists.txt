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

target_sources(kboot PRIVATE ${KERNEL_BIN})
target_include_directories(kboot PRIVATE ../kernel)
target_link_libraries(kboot PRIVATE stdbigos)

COMPILE_BINARY(kboot)
