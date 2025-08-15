SETUP_EXECUTABLE(kernel)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(kernel PRIVATE __DEBUG__)
endif()

target_link_libraries(kernel PRIVATE stdbigos)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(kernel PRIVATE Debug)
endif()

target_link_options(kernel PRIVATE -pie -e kinit)
target_compile_options(kernel PRIVATE -fPIE -static -std=gnu23 -D__LOGLVL__=3)

COMPILE_BINARY(kernel)
ADD_QEMU_TARGET(kernel)
