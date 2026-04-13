include(FetchContent)

FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(unity)

add_subdirectory(kernel/memory_management/physical_memory)
