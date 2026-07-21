cmake_minimum_required(VERSION 3.29)
project(BigOS LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(SYSTEM external/include)
include_directories(include)

include(CMake/CommonConfig.cmake)

option(BIGOS_BUILD_TESTS "Build unit tests (native host only)" OFF)
if(BIGOS_BUILD_TESTS)
	include(CMake/TestsConfig.cmake)
	enable_testing()
	add_subdirectory(tests)
	add_subdirectory(src/libcore)
	add_subdirectory(src/libboot)
else()
	include(CMake/Linters.cmake)
	add_subdirectory(src)
	add_subdirectory(external/efi)
endif()
