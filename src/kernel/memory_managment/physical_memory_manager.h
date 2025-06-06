#ifndef BIGOS_KERNEL_MEMORY_MANAGER_PMM
#define BIGOS_KERNEL_MEMORY_MANAGER_PMM

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef u64 ppn_t;
typedef u64 phys_addr_t;

typedef struct {
	phys_addr_t start;
	size_t size;
} phys_mem_region_t;

typedef enum : u8 {
	PAGE_SIZE_4kB = 0,   // kilo-page
	PAGE_SIZE_2MB = 1,   // mega-page
	PAGE_SIZE_1GB = 2,   // giga-page
	PAGE_SIZE_512GB = 3, // tera-page
	PAGE_SIZE_256TB = 4, // peta-page
} page_size_t;

typedef struct {
	size_t count;
	phys_mem_region_t regions[];
} phys_buffer_t; //starts at 0x0 and not at the address where ram is mapped

error_t phys_mem_init();
error_t phys_mem_alloc_frame(page_size_t ps, ppn_t ppnOUT);
error_t phys_mem_free_frame(ppn_t ppn);
error_t phys_mem_find_free_region(u64 alignment, phys_buffer_t busy_regions, phys_mem_region_t* regionOUT);

#endif // !BIGOS_KERNEL_MEMORY_MANAGER_PMM
