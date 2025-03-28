#ifndef _KERNEL_VIRTUAL_MEMORY_VMM_H_
#define _KERNEL_VIRTUAL_MEMORY_VMM_H_

#include <stdbigos/error.h>
#include "stdbigos/types.h"

typedef enum {
	PAGE_SIZE_4K, PAGE_SIZE_2M, PAGE_SIZE_1G, PAGE_SIZE_512G,
} PAGE_SIZE_t;

typedef enum {
	VSM_Disable = 0, VMS_Sv39 = 8, VMS_Sv48 = 9, VMS_Sv57 = 10, VMS_Sv64 = 11,
} VIRT_MEM_SCHEME_t;

typedef u16 asid_t;
#define ASID_MAX UINT16_MAX

ERROR virtual_memory_init(VIRT_MEM_SCHEME_t vm_scheme, asid_t asid);

#endif //_KERNEL_VIRTUAL_MEMORY_VMM_H_
