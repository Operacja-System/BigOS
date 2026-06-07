SETUP_UNIT_TEST(pmallocator_test
    ${PROJECT_SOURCE_DIR}/src/kernel/memory_management/physical_memory/allocator.c
)

target_include_directories(pmallocator_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src/kernel
)

SETUP_UNIT_TEST(hal_memory_regions_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src/kernel/hal/arch/riscv/memory_region.c)

target_include_directories(test_hal_memory_regions PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/include
    ${PROJECT_SOURCE_DIR}/src/kernel
)
