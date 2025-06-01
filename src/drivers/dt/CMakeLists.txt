SETUP_LIBRARY(dt)

target_link_libraries(dt PRIVATE stdbigos)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(dt PRIVATE Debug)
endif()


target_include_directories(dt
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/drivers/dt
)
