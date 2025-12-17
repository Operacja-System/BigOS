#ifndef _STDBIGOS_MATH_H
#define _STDBIGOS_MATH_H

#define ALIGN_UP(x, align)       (((x) + (align) - 1) / (align) * (align))
#define ALIGN_UP_POW2(x, pow2)   (((x) + (1 << (pow2)) - 1) & ~((1 << (pow2)) - 1))
#define ALIGN_DOWN(x, align)     (((x) / (align) * (align)))
#define ALIGN_DOWN_POW2(x, pow2) (((x) & ~((1 << pow2) - 1)))

#define POW2(x) (1 << x)

#ifndef min
	#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
	#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#endif
