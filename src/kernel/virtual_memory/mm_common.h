#ifndef _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H
#define _KERNEL_VIRTUAL_MEMORY_MM_COMMON_H

#include <stdbigos/types.h>

typedef enum : u8 {
	PAGE_SIZE_4K = 0,
	PAGE_SIZE_2M = 1,
	PAGE_SIZE_1G = 2,
	PAGE_SIZE_512G = 3,
	PAGE_SIZE_256T = 4,
} page_size_t;

static const u64 page_size_in_bytes[] = {0x1000ull, 0x200000ull, 0x40000000ull, 0x8000000000ull, 0x800000000000ull};

typedef u64 phys_addr_t;
typedef void* virt_addr_t;
typedef u64 virtual_page_number_t;
typedef u64 physical_page_number_t;

[[nodiscard]] virt_addr_t physical_to_virtual(phys_addr_t paddr);

#endif // !_KERNEL_VIRTUAL_MEMORY_MM_COMMON_H
