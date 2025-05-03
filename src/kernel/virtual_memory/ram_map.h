#ifndef _KERNEL_VIRTUAL_MEMORY_RAM_MAP_H_
#define _KERNEL_VIRTUAL_MEMORY_RAM_MAP_H_

#include "mm_common.h"

void set_ram_map_address(void* ram_addr);
void* get_effective_address(ppn_t ppn, u64 offset);

#endif // !_KERNEL_VIRTUAL_MEMORY_RAM_MAP_H_
