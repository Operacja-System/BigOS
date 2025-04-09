#ifndef _KERNEL_VIRTUAL_MEMORY_VMM_H_
#define _KERNEL_VIRTUAL_MEMORY_VMM_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"

typedef u16 asid_t;

typedef enum {
	VMS_BARE = 0,
	VMS_Sv39 = 1,
	VMS_Sv48 = 2,
	VMS_Sv57 = 3,
	VMS_Sv64 = 4, // NOTE: This is not yet implemented by riscv ISA (09-04-2025)
} virt_mem_scheme_t;

[[nodiscard]] error_t virtual_memory_init(void* RAM_start);
[[nodiscard]] error_t virtual_memory_enable(virt_mem_scheme_t vms, asid_t asid);
[[nodiscard]] error_t virtual_memory_disable();

[[nodiscard]] asid_t get_asid_max_val();
[[nodiscard]] virt_mem_scheme_t get_active_virt_mem_scheme();
[[nodiscard]] const char* get_virt_mem_scheme_str_name(virt_mem_scheme_t vms);

[[nodiscard]] error_t resolve_page_fault();

#endif //_KERNEL_VIRTUAL_MEMORY_VMM_H_
