#ifndef BIGOS_KERNEL_MEMORY_MANAGER_PMM
#define BIGOS_KERNEL_MEMORY_MANAGER_PMM

#include <stdbigos/error.h>

#include "mm_types.h"

typedef struct {
	size_t count;
	phys_mem_region_t* regions;
} phys_buffer_t;

[[nodiscard]]
error_t phys_mem_init(phys_buffer_t busy_regions);

[[nodiscard]]
error_t phys_mem_alloc_frame(page_size_t ps, ppn_t* ppnOUT);

[[nodiscard]]
error_t phys_mem_free_frame(ppn_t ppn);

#endif // !BIGOS_KERNEL_MEMORY_MANAGER_PMM
