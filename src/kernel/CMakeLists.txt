SETUP_EXECUTABLE(kernel)

target_link_libraries(kernel PRIVATE stdbigos)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(kernel PRIVATE Debug)
endif()
