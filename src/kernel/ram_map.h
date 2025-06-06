#ifndef BIGOS_KERNEL_RAM_MAP
#define BIGOS_KERNEL_RAM_MAP

#include <stdbigos/types.h>
#include "memory_managment/physical_memory_manager.h"

typedef struct {
	void* start;
	size_t size;
} ram_map_data_t;

void set_ram_map_data(ram_map_data_t data);
void* physical_to_effective(phys_addr_t paddr);

#endif // !BIGOS_KERNEL_RAM_MAP
