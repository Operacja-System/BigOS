#include <stdbigos/math.h>

u64 align_up(u64 val, u64 alignment) {
	return CEIL_DIV(val, alignment);
}

u64 align_up_pow2(u64 val, u64 pow) {
	const u64 align = (1 << pow) - 1;
	return (val + align) & ~align;
}
