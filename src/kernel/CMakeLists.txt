SETUP_EXECUTABLE(kernel)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(kernel PRIVATE __DEBUG__)
endif()

target_link_libraries(kernel PRIVATE stdbigos)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(kernel PRIVATE Debug)
endif()

target_link_options(kernel PRIVATE -no-pie -T ${LINKER_SCRIPT})
target_compile_options(kernel PRIVATE -fno-pie -static)

COMPILE_BINARY(kernel)
ADD_QEMU_TARGET(kernel)
