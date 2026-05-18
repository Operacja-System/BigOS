add_executable(test_initramfs
    initramfs.c
    ${PROJECT_SOURCE_DIR}/src/kernel/hal/initramfs.c
)

target_compile_features(test_pmallocator PUBLIC c_std_23)
target_compile_options(test_pmallocator PRIVATE -Wall -Wextra)

target_include_directories(test_pmallocator PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/include
    ${PROJECT_SOURCE_DIR}/src/kernel
    ${PROJECT_SOURCE_DIR}/src/kernel/hal/include
)

target_link_libraries(test_initramfs PRIVATE unity)

add_test(NAME initramfs_tests COMMAND test_initramfs)
