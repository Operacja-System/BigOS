#ifndef _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_
#define _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_

#include <stdbigos/types.h>

typedef enum : u8 {
	PAGE_SIZE_4kB = 0,	 // kilo-page
	PAGE_SIZE_2MB = 1,	 // Mega-page
	PAGE_SIZE_1GB = 2,	 // Giga-page
	PAGE_SIZE_512GB = 3, // Tera-page
	PAGE_SIZE_256TB = 4, // Peta-page
	PAGE_SIZE_MAX = PAGE_SIZE_256TB,
	PAGE_SIZE_AMOUNT
} page_size_t;

typedef enum {
	VMS_BARE = -1,
	VMS_SV_39 = 0,
	VMS_SV_48 = 1,
	VMS_SV_57 = 2,
} virtual_memory_scheme_t;

typedef u64 vpn_t;
typedef u64 ppn_t;

#endif // !_KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_
