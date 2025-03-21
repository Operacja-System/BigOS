cmake_minimum_required(VERSION 3.21)
project(BigOS LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMake/CommonConfig.cmake)
add_subdirectory(src)
