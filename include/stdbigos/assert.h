#ifndef STDBIGOS_ASSERT_H
#define STDBIGOS_ASSERT_H
#include <stdbigos/error.h>
#ifndef kassert
	#define kassert(expr)                          \
		do {                                       \
			if (!(expr)) {                         \
				PANIC("Assertion failed: " #expr); \
			}                                      \
		} while (0)
#endif
#endif
