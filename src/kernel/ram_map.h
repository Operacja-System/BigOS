#ifndef BIGOS_KERNEL_RAM_MAP
#define BIGOS_KERNEL_RAM_MAP

#include <stdbigos/types.h>

#include "memory_managment/physical_memory_manager.h"

typedef struct {
	void* addr;
	phys_addr_t phys_addr;
	size_t size;
} ram_map_data_t;

void ram_map_set_data(ram_map_data_t data);
[[nodiscard]] error_t ram_map_get_data(ram_map_data_t* dataOUT);
void* physical_to_effective(phys_addr_t paddr);

#endif // !BIGOS_KERNEL_RAM_MAP
