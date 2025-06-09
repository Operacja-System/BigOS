#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT
#define BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT

#include <stdbigos/types.h>
#include "memory_managment/physical_memory_manager.h"

typedef enum : u8 {
	VMS_BARE = 0,
	VMS_SV39 = 8,
	VMS_SV48 = 9,
	VMS_SV57 = 10,
} virt_mem_scheme_t;

[[nodiscard]] u16 virt_mem_get_max_asid();
void virt_mem_set_satp(u16 asid, virt_mem_scheme_t vms, ppn_t page_table);

#endif // !BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT
