SETUP_EXECUTABLE(kernel)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(kernel PRIVATE __DEBUG__)
endif()

target_link_libraries(kernel PRIVATE stdbigos)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(kernel PRIVATE Debug)
endif()
