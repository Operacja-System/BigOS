#ifndef _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_
#define _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_

#include <stdbigos/types.h>

typedef u64 vpn_t;
typedef u64 ppn_t;

typedef enum : u8 {
	PAGE_SIZE_4kB,	 // kilo-page
	PAGE_SIZE_2MB,	 // Mega-page
	PAGE_SIZE_1GB,	 // Giga-page
	PAGE_SIZE_512GB, // Tera-page
	PAGE_SIZE_256TB, // Peta-page
} page_size_t;

#endif				 // !_KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_
