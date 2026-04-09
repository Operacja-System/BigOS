SETUP_EXECUTABLE(example_trap)

target_link_libraries(example_trap PRIVATE startup hal Debug stdbigos)

ADD_QEMU_TARGET(example_trap)
