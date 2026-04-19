SETUP_LIBRARY(startup)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_libraries(startup PRIVATE hal relocations stdbigos)
target_link_options(startup PUBLIC -static-pie -T ${LINKER_SCRIPT})
set_target_properties(startup PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
