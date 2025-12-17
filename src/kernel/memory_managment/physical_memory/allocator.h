#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_ALLOCATOR
#define BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_ALLOCATOR

#include <memory_managment/include/physical_memory/manager.h>
#include <stdbigos/error.h>

[[nodiscard]]
size_t pmallocator_get_header_size(memory_area_t area);

error_t pmallocator_init_region(memory_area_t area, memory_region_t header, memory_area_t* res_areas, u32 count);

error_t pmallocator_allocate(u8 fs, phys_addr_t* addrOUT, memory_area_t area, memory_region_t header);

error_t pmallocator_free(phys_addr_t addr, memory_area_t area, memory_region_t header);

#endif
