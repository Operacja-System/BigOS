SETUP_UNIT_TEST(pmallocator_test
    test_allocator.c
    ${PROJECT_SOURCE_DIR}/src/kernel/memory_management/physical_memory/allocator.c
)

target_include_directories(pmallocator_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src/kernel
)
