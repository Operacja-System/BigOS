SETUP_UNIT_TEST(pmallocator_test
    ${PROJECT_SOURCE_DIR}/src/kernel/memory_management/physical_memory/allocator.c
)

target_include_directories(pmallocator_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src/kernel
)
