#ifndef _KERNEL_VIRTUAL_MEMORY_PMM_H_
#define _KERNEL_VIRTUAL_MEMORY_PMM_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include "mm_common.h"

error_t allocate_page_frame(page_size_t page_size, ppn_t* ppnOUT);
error_t free_page_frame(ppn_t ppn);
error_t set_phisical_memory_region_busy(ppn_t ppn, u64 size_in_bytes);


#endif // !_KERNEL_VIRTUAL_MEMORY_PMM_H_
