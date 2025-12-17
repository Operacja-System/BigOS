#include "allocator.h"

size_t pmallocator_get_header_size(memory_area_t area) {
	(void)area;
	return 0;
}

error_t pmallocator_init_region(memory_area_t area, memory_region_t header, memory_area_t* res_areas, u32 count) {
	(void)area;
	(void)header;
	(void)res_areas;
	(void)count;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_allocate(u8 fs, phys_addr_t* addrOUT, memory_area_t area, memory_region_t header) {
	(void)fs;
	*addrOUT = *addrOUT;
	(void)area;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_free(phys_addr_t addr, memory_area_t area, memory_region_t header) {
	(void)addr;
	(void)area;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}
