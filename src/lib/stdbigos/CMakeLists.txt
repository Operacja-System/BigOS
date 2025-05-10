SETUP_LIBRARY(stdbigos-impl)

target_link_libraries(stdbigos-impl PUBLIC libc::libc)

add_library(stdbigos INTERFACE)
target_link_libraries(stdbigos INTERFACE "$<LINK_GROUP:RESCAN,stdbigos-impl,libc::libc>")
