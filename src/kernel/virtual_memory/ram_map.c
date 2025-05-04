#include "ram_map.h"

static void* ram_map_address = nullptr;

void set_ram_map_address(void* ram_addr) {
	ram_map_address = ram_addr;
}

void* get_effective_address(ppn_t ppn, u64 offset) {
	return (void*)((uintptr_t)ram_map_address + (ppn << 12) + offset);
}
