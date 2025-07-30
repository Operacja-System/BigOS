#include "ram_map.h"

#include <debug/debug_stdio.h>

static ram_map_data_t ram_data = {0};
static bool is_set = false;

void ram_map_set_data(ram_map_data_t data) {
#ifdef __DEBUG__
	if (is_set) [[clang::unlikely]]
		DEBUG_PRINTF("ram map data changed to addr: %lx, size: 0x%lx\n", (u64)data.addr, data.size);
#endif
	ram_data = data;
	is_set = true;
}

error_t ram_map_get_data(ram_map_data_t* dataOUT) {
	if (!is_set)
		return ERR_NOT_VALID;
	*dataOUT = ram_data;
	return ERR_NONE;
}

void* physical_to_effective(phys_addr_t paddr) {
#ifdef __DEBUG__
	if (!is_set) [[clang::unlikely]]
		DEBUG_PRINTF("ERROR: physical_to_effective was called before set_ram_map_data\n");
	if (paddr > ram_data.size) [[clang::unlikely]]
		DEBUG_PRINTF("ERROR: phys_addr exceeded ram map size\n");
#endif
	return ram_data.addr + paddr;
}
