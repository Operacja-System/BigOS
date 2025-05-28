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

typedef enum : u8 {
	PTEF_V = (1u << 0u),
	PTEF_R = (1u << 1u),
	PTEF_W = (1u << 2u),
	PTEF_X = (1u << 3u),
	PTEF_U = (1u << 4u),
	PTEF_G = (1u << 5u),
	PTEF_A = (1u << 6u),
	PTEF_D = (1u << 7u),
} page_table_entry_flags_t;

typedef u64 vpn_t;
typedef u64 ppn_t;

#endif // !_KERNEL_VIRTUAL_MEMORY_MM_COMMON_H_
