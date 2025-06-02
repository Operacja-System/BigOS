#ifndef _KERNEL_VIRTUAL_MEMORY_PMM_H_
#define _KERNEL_VIRTUAL_MEMORY_PMM_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"
#include "vmm.h"

[[nodiscard]] error_t initialize_pmm(virtual_memory_scheme_t vms, u64 ram_size);
[[nodiscard]] error_t allocate_page_frame(page_size_t page_size, ppn_t* ppnOUT);
[[nodiscard]] error_t free_page_frame(ppn_t ppn);
[[nodiscard]] error_t set_physical_memory_region_busy(ppn_t ppn, u64 size_in_bytes);

#endif // !_KERNEL_VIRTUAL_MEMORY_PMM_H_
