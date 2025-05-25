SETUP_LIBRARY(Drivers)
target_link_libraries(Drivers PRIVATE stdbigos)

add_subdirectory(dt)

target_link_libraries(Drivers PRIVATE stdbigos dt)
