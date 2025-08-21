#ifndef BIGOS_KERNEL_DEVTREE_DEVTREE_TYPES
#define BIGOS_KERNEL_DEVTREE_DEVTREE_TYPES

#include <stdbigos/types.h>
#include <stdbigos/buffer.h>

typedef const char* dtds_string_t;

struct {
	const char* stringlist;
	u64 count;
} dtds_stringlist_t;

typedef u32 dtds_phandle_t;


#endif
