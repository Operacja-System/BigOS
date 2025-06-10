#ifndef BIGOS_KERNEL_MEMORY_MANAGER_PMM
#define BIGOS_KERNEL_MEMORY_MANAGER_PMM

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_types.h"

typedef struct {
	size_t count;
	phys_mem_region_t* regions;
} phys_buffer_t;

[[nodiscard]] error_t phys_mem_init();
[[nodiscard]] error_t phys_mem_alloc_frame(page_size_t ps, ppn_t* ppnOUT);
[[nodiscard]] error_t phys_mem_free_frame(ppn_t ppn);
[[nodiscard]] error_t phys_mem_block_region(phys_mem_region_t region);
[[nodiscard]] error_t phys_mem_find_free_region(u64 alignment, phys_buffer_t busy_regions,
                                                phys_mem_region_t* regionOUT);

#endif // !BIGOS_KERNEL_MEMORY_MANAGER_PMM
