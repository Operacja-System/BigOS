#ifndef _KERNEL_VIRTUAL_MEMORY_PMM_H_
#define _KERNEL_VIRTUAL_MEMORY_PMM_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"

[[nodiscard]] error_t alloc_frame(page_size_t psize, physical_page_number_t* ppnOUT);
void pmm_set_memory_regions_as_reserved(); // TODO: Implement (it doesn't have to be called this, this is just an
										   // information that such function needs to be implemented)
void free_frame(physical_page_number_t ppn);

#endif //!_KERNEL_VIRTUAL_MEMORY_PMM_H_
