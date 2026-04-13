add_executable(test_pmallocator
    test_allocator.c
    ${PROJECT_SOURCE_DIR}/src/kernel/memory_management/physical_memory/allocator.c
    ${PROJECT_SOURCE_DIR}/tests/stubs/hal_irq_stub.c
)

target_compile_features(test_pmallocator PUBLIC c_std_23)
target_compile_options(test_pmallocator PRIVATE -Wall -Wextra)

target_include_directories(test_pmallocator PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/include
    ${PROJECT_SOURCE_DIR}/src/kernel
    ${PROJECT_SOURCE_DIR}/src/kernel/memory_management/physical_memory
)

target_link_libraries(test_pmallocator PRIVATE unity)

add_test(NAME pmallocator_tests COMMAND test_pmallocator)
