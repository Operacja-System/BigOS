#ifndef LIBCORE_ARRAY_SIZES
#define LIBCORE_ARRAY_SIZES

#if __has_attribute(__counted_by__)
	#define __counted_by(member) __attribute__((__counted_by__(member)))
#else
	#define __counted_by(member)
#endif

#if __has_attribute(__sized_by__)
	#define __sized_by(member) __attribute__((__sized_by__(member)))
#else
	#define __sized_by(member)
#endif

#endif // !LIBCORE_ARRAY_SIZES
