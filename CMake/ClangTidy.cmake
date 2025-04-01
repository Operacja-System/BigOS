set(BIGOS_USE_CLANG_TIDY OFF CACHE BOOL "Check using clang-tidy during build")

if (BIGOS_USE_CLANG_TIDY)
    set(CMAKE_C_CLANG_TIDY clang-tidy)
endif()
