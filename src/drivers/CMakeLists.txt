add_subdirectory(dt)

SETUP_LIBRARY(drivers)
target_link_libraries(drivers PRIVATE stdbigos)
