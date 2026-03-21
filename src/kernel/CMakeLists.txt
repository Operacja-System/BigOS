SETUP_EXECUTABLE(kernel)

target_compile_definitions(kernel PRIVATE $<IF:$<CONFIG:DEBUG>,__DEBUG__ __LOGLVL__=3, __LOGLVL__=1>)
target_compile_definitions(kernel PRIVATE __ARCH_RISCV__) # Make this an opt when we add support for other arches
target_link_libraries(kernel PRIVATE Debug stdbigos)

target_link_options(kernel PRIVATE -static-pie -e kinit)

ADD_QEMU_TARGET(kernel)
