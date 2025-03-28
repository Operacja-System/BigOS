#ifndef _KERNEL_VIRTUAL_MEMORY_VMM_H_
#define _KERNEL_VIRTUAL_MEMORY_VMM_H_

#include <stdbigos/error.h>

#include "stdbigos/types.h"

typedef enum {
	PAGE_SIZE_4K,
	PAGE_SIZE_2M,
	PAGE_SIZE_1G,
	PAGE_SIZE_512G,
} page_size_t;

typedef enum {
	VMS_DISABLE = 0,
	VMS_Sv39 = 8,
	VMS_Sv48 = 9,
	VMS_Sv57 = 10,
	VMS_Sv64 = 11,
} virt_mem_scheme_t;

typedef u16 asid_t;

[[nodiscard]] error_t virtual_memory_init(virt_mem_scheme_t vms, asid_t asid);
[[nodiscard]] asid_t get_asid_max_val();

#endif //_KERNEL_VIRTUAL_MEMORY_VMM_H_
