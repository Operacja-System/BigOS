#ifndef BIGOS_INCLUDE_STDBIGOS_MATH
#define BIGOS_INCLUDE_STDBIGOS_MATH

#include <stdbigos/types.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CEIL_DIV(val, div) (u64)(((val) + (div) - 1) / (div))

u64 align_up(u64 val, u64 alignment);

u64 align_up_pow2(u64 val, u64 pow);

#endif // !BIGOS_INCLUDE_STDBIGOS_MATH
