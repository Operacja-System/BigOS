#ifndef _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H
#define _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H

#include <stdbigos/types.h>

typedef enum : u8 {
	PAGE_SIZE_4K,
	PAGE_SIZE_2M,
	PAGE_SIZE_1G,
	PAGE_SIZE_512G,
	PAGE_SIZE_256T,
} page_size_t; // NOTE: The order of those enum values is important

typedef u64 phys_addr_t;

#endif // !_KERNEL_VIRTUAL_MEMORY_MM_COMMON_H
