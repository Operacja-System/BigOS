#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT
#define BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT

#include <stdbigos/types.h>

#include "mm_types.h"

[[nodiscard]]
u16 virt_mem_get_max_asid();

void virt_mem_set_satp(u16 asid, u8 vms, ppn_t page_table);

void virt_mem_flush_TLB();

void virt_mem_flush_TLB_entry(void* va);

void virt_mem_flush_TLB_address_space(u64 asid);

#endif // !BIGOS_KERNEL_MEMORY_MANAGMENT_VIRTUAL_MEMORY_MANAGMENT
