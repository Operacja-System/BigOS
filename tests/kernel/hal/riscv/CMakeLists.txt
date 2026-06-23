SETUP_UNIT_TEST(hal_memory_regions_test
    test_hal_memory_regions.c
    ${PROJECT_SOURCE_DIR}/src/kernel/hal/arch/riscv/memory_regions.c)

target_include_directories(hal_memory_regions_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/include
    ${PROJECT_SOURCE_DIR}/src/kernel
)
