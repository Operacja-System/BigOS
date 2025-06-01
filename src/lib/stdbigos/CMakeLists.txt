SETUP_LIBRARY(stdbigos)

# This library contains low level functions such as memcpy. As such it is
# optimized for speed. We also define no-tree-loop-distribute-patterns to
# prevent compiler from "optimizing" those functions into a recursive call

target_compile_options(stdbigos PRIVATE
    -O3
    $<$<C_COMPILER_ID:GNU>:-fno-tree-loop-distribute-patterns>
)
