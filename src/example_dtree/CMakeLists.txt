SETUP_EXECUTABLE(example_dtree)

target_link_libraries(example_dtree PRIVATE dt_access Debug startup stdbigos)

ADD_QEMU_TARGET(example_dtree)
