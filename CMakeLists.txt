cmake_minimum_required(VERSION 3.24)
project(BigOS LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(SYSTEM external/include)
include_directories(include)

include(CMake/CommonConfig.cmake)
add_subdirectory(src)
