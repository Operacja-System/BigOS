#ifndef LIBCORE_MATH
#define LIBCORE_MATH

#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#define ALIGN_UP(x, align)   (ALIGN_DOWN((x) + (align) - 1, (align)))

#define EXP2(x) (1ull << (x))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif // !LIBCORE_MATH
