add_subdirectory(dt)

SETUP_LIBRARY(drivers)
target_link_libraries(drivers INTERFACE dt)

target_include_directories(drivers
  INTERFACE
    ${CMAKE_SOURCE_DIR}/include/drivers
)
