cmake_minimum_required(VERSION 3.29)
project(BigOS LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(SYSTEM external/include)
include_directories(include)

option(BIGOS_BUILD_TESTS "Build unit tests (native host only)" OFF)
if(BIGOS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
else()
  include(CMake/Linters.cmake)
  include(CMake/CommonConfig.cmake)
  add_subdirectory(src)
endif()
