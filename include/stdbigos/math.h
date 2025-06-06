#ifndef BIGOS_INCLUDE_STDBIGOS_MATH
#define BIGOS_INCLUDE_STDBIGOS_MATH

#include <stdbigos/types.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CEIL_DIV(val, div) (u64)(((val) + (div) - 1) / (div))

inline u64 align_up(u64 val, u64 alignment) { return CEIL_DIV(val, alignment); }
inline u64 align_up_pow2(u64 val, u64 pow) { const u64 align = (1 << pow) - 1; return (val + align) & ~align; }

#endif // !BIGOS_INCLUDE_STDBIGOS_MATH
