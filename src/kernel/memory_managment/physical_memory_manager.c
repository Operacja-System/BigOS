#include "physical_memory_manager.h"
#include <stdbigos/math.h>

error_t phys_mem_init() {

}

error_t phys_mem_alloc_frame(page_size_t ps, ppn_t* ppnOUT) {

}

error_t phys_mem_free_frame(ppn_t ppn) {

}

error_t phys_mem_find_free_region(u64 alignment, phys_buffer_t busy_regions, phys_mem_region_t* regionOUT) {
	phys_buffer_t reserved_regions = {0};
	//TODO: Read reserved regions from device tree
	u64 ram_size = 0; //TODO: Read ram size from device tree
	phys_buffer_t unavalible_regions[2] = {reserved_regions, busy_regions};
	phys_addr_t curr_region_start = 0;
	bool overlap = false;
	while(curr_region_start + regionOUT->size < ram_size) {
		for(u32 buff_idx = 0; buff_idx < sizeof(unavalible_regions) / sizeof(unavalible_regions[0]); ++buff_idx) {
			for(size_t reg_idx = 0; reg_idx < unavalible_regions[buff_idx].count; ++reg_idx) {
				phys_addr_t unavalible_region_start = unavalible_regions[buff_idx].regions[reg_idx].start;
				phys_addr_t unavalible_region_end = unavalible_region_start + unavalible_regions[buff_idx].regions[reg_idx].size;
				phys_addr_t curr_region_end = curr_region_start + regionOUT->size;
					if(MAX(curr_region_start, unavalible_region_start) < MIN(curr_region_end, unavalible_region_end)) {
						curr_region_start = unavalible_region_end;
						curr_region_start = (phys_addr_t)align_up((u64)curr_region_start, alignment);
						overlap = true;
						break;
					}
			}
			if(overlap) break;
		}
		if(!overlap) {
			regionOUT->start = curr_region_start;
			return ERR_NONE;
		}
	}
	return ERR_PHYSICAL_MEMORY_FULL;
}
