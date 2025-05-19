SETUP_EXECUTABLE(kboot)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(kboot PRIVATE -T ${LINKER_SCRIPT})
set_target_properties(kboot PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(kboot PRIVATE )
target_compile_definitions(kboot PRIVATE __DEBUG__)

COMPILE_BINARY(kboot)
ADD_QEMU_TARGET(kboot BIOS_IMAGE)
