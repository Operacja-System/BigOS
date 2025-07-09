#include "physical_memory_manager.h"
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include "ram_map.h"

typedef struct {
	u16 avalible_kiloframes;
	bool busy;
} megaframe_data_t;

typedef struct {
	u32 avalible_kiloframes;
	u16 avalible_megaframes;
	bool busy;
} gigaframe_data_t;

static bool s_is_init = false;
static phys_addr_t s_kiloframe_bitmap = 0;
static phys_addr_t s_megaframe_data = 0;
static phys_addr_t s_gigaframe_data = 0;

error_t phys_mem_init(phys_buffer_t busy_regions) {
	if(s_is_init) return ERR_NOT_VALID;
	ram_map_data_t ram_data = {0};
	error_t err = ram_map_get_data(&ram_data);
	if(err) return ERR_INTERNAL_FAILURE;
	size_t kiloframe_bitmap_size = ram_data.size << 18; // ram size in GB * 2^30 / 2^12 (4kB)
	size_t megaframe_data_size = (ram_data.size << 9);
	size_t gigaframe_data_size = (ram_data.size);
	size_t alloc_size =
		kiloframe_bitmap_size +
		(megaframe_data_size * sizeof(megaframe_data_t)) +
		(gigaframe_data_size * sizeof(gigaframe_data_t));
	phys_mem_region_t mem_reg = {.size = alloc_size, .addr = 0};
	err = phys_mem_find_free_region(0x1000, busy_regions, &mem_reg);
	if(err) return ERR_INTERNAL_FAILURE;
	s_kiloframe_bitmap = mem_reg.addr;
	s_megaframe_data = s_kiloframe_bitmap + kiloframe_bitmap_size;
	s_gigaframe_data = s_megaframe_data + megaframe_data_size;

	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	memset(kiloframe_bitmap, 0, kiloframe_bitmap_size);
	for(u64 i = 0; i < megaframe_data_size; ++i)
		megaframe_data[i] = (megaframe_data_t){.avalible_kiloframes = 512, .busy = false};
	for(u64 i = 0; i < gigaframe_data_size; ++i)
		gigaframe_data[i] = (gigaframe_data_t){.avalible_kiloframes = 262144, .avalible_megaframes = 512, .busy = false};

	phys_mem_region_t busy_regions_data[busy_regions.count + 1];
	memcpy(busy_regions_data, busy_regions.regions, busy_regions.count * sizeof(phys_mem_region_t));
	busy_regions_data[busy_regions.count] = mem_reg;
	busy_regions.regions = busy_regions_data;
	++busy_regions.count;
	err = phys_mem_announce_busy_regions(busy_regions);
	if(err) return ERR_INTERNAL_FAILURE;
	s_is_init = true;
	return ERR_NONE;
}

error_t phys_mem_announce_busy_regions(phys_buffer_t regions) {
	if (!s_is_init) return ERR_NOT_INITIALIZED;
	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	ram_map_data_t ram_data = {0};
	error_t err = ram_map_get_data(&ram_data);
	if (err) return ERR_INTERNAL_FAILURE;
	for(u64 i = 0; i < regions.count; ++i) {
		ppn_t region_pn = (regions.regions[i].addr - ram_data.phys_addr) >> 12;
		size_t occupied_frames_count = (regions.regions[i].size >> 12) + (regions.regions[i].size & 0x2ff) != 0;
		for (size_t ppn = region_pn; ppn < region_pn + occupied_frames_count; ++ppn) {
			kiloframe_bitmap[ppn >> 6] |= 1ull << (ppn & 0x2f);
			--megaframe_data[ppn >> 9].avalible_kiloframes;
			--gigaframe_data[ppn >> 18].avalible_kiloframes;
			if(megaframe_data[ppn >> 9].avalible_kiloframes == 511)
				--gigaframe_data[ppn >> 18].avalible_megaframes;
		}
	}
	return ERR_NONE;
}

error_t phys_mem_alloc_frame(page_size_t ps, ppn_t* ppnOUT) {

}

error_t phys_mem_free_frame(ppn_t ppn) {
	if(!s_is_init) return ERR_NOT_INITIALIZED;

}

error_t phys_mem_find_free_region(u64 alignment, phys_buffer_t busy_regions, phys_mem_region_t* regOUT) {
	phys_buffer_t reserved_regions = {0};
	//TODO: Read reserved regions from device tree
	u64 ram_size = 0; //TODO: Read ram size from device tree
	phys_buffer_t unavalible_regions[2] = {reserved_regions, busy_regions};
	phys_addr_t curr_region_start = 0;
	bool overlap = false;
	while(curr_region_start + regOUT->size < ram_size) {
		for(u32 buff_idx = 0; buff_idx < sizeof(unavalible_regions) / sizeof(unavalible_regions[0]); ++buff_idx) {
			for(size_t reg_idx = 0; reg_idx < unavalible_regions[buff_idx].count; ++reg_idx) {
				phys_addr_t unavalible_region_start = unavalible_regions[buff_idx].regions[reg_idx].addr;
				phys_addr_t unavalible_region_end = unavalible_region_start + unavalible_regions[buff_idx].regions[reg_idx].size;
				phys_addr_t curr_region_end = curr_region_start + regOUT->size;
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
			regOUT->addr = curr_region_start;
			return ERR_NONE;
		}
	}
	return ERR_PHYSICAL_MEMORY_FULL;
}
