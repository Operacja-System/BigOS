SETUP_LIBRARY(dt)

target_link_libraries(dt PRIVATE stdbigos)


target_include_directories(dt
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/drivers/dt   # for dt.h / dt_props.h
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}              # for dt_node.h, dt_alloc.h, dt_parser.h
)
