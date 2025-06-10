#ifndef BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES
#define BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES

#include <stdbigos/types.h>

typedef u64 vpn_t;
typedef u64 ppn_t;
typedef u64 phys_addr_t;

typedef enum : u8 {
	PAGE_SIZE_4kB = 0,   // kilo-page
	PAGE_SIZE_2MB = 1,   // mega-page
	PAGE_SIZE_1GB = 2,   // giga-page
	PAGE_SIZE_512GB = 3, // tera-page
	PAGE_SIZE_256TB = 4, // peta-page
} page_size_t;

typedef struct {
	phys_addr_t addr;
	size_t size;
} phys_mem_region_t;

typedef struct {
	bool mapped;
	bool read;
	bool write;
	bool execute;
	bool user;
	bool global;
	page_size_t ps;
	void* addr;
	size_t size;
	phys_mem_region_t map_region;
} virt_mem_region_t;

#endif // !BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES
