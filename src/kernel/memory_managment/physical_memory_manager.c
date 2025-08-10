#include "physical_memory_manager.h"

#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdint.h>

#include "ram_map.h"
#include "stdbigos/error.h"

typedef struct {
	u16 available_kiloframes;
	bool allocated;
} megaframe_data_t;

typedef struct {
	u32 available_kiloframes;
	u16 available_megaframes;
	bool allocated;
} gigaframe_data_t;

#ifdef __DEBUG__
static bool s_is_init = false;
#endif

/* NOTE:
 * kiloframe = 4kB = 4 * (2 ^ 10)B
 * megaframe = 2MB = 2 * (2 ^ 20)B
 * gigaframe = 1GB = 1 * (2 ^ 30)B
 * */
static phys_addr_t s_kiloframe_bitmap = 0;
static phys_addr_t s_megaframe_data = 0;
static phys_addr_t s_gigaframe_data = 0;

static bool read_bitmap(u64 idx, const u64* bitmap) {
	return ((bitmap[idx >> 6] >> (idx & 0x3f)) & 1) != 0ull;
}

static void set_bitmap(bool v, u64 idx, u64* bitmap) {
	bitmap[idx >> 6] &= ~(1ull << (idx & 0x3f));
	bitmap[idx >> 6] |= ((u64)v << (idx & 0x3f));
}

static error_t scan_for_suitable_GB_frame(page_size_t ps, u64* found) {
	ram_map_data_t ram_data = {0};
	(void)ram_map_get_data(&ram_data); // Error check not needed, already checked at PMM init
	bool found_suitable = false;
	u64 minimum = UINT64_MAX;
	u64 found_idx = 0;
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	for (u64 i = 0; i < ram_data.size_GB; ++i) {
		if (!gigaframe_data[i].allocated)
			continue;
		switch (ps) {
		case PAGE_SIZE_4kB:
			if (gigaframe_data[i].available_kiloframes != 0 && gigaframe_data[i].available_kiloframes < minimum) {
				minimum = gigaframe_data[i].available_kiloframes;
				found_idx = i;
				found_suitable = true;
			}
			break;
		case PAGE_SIZE_2MB:
			if (gigaframe_data[i].available_megaframes != 0 && gigaframe_data[i].available_megaframes < minimum) {
				minimum = gigaframe_data[i].available_megaframes;
				found_idx = i;
				found_suitable = true;
			}
			break;
		case PAGE_SIZE_1GB: *found = i; return ERR_NONE;
		default:            return ERR_PHYSICAL_MEMORY_FULL;
		}
	}
	if (!found_suitable)
		return ERR_PHYSICAL_MEMORY_FULL;
	*found = found_idx;
	return ERR_NONE;
}

static error_t scan_for_suitable_MB_frame(page_size_t ps, u64 GB_frame_idx, u64* found) {
	bool found_suitable = false;
	u64 minimum = UINT64_MAX;
	u64 found_idx = 0;
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	for (u64 idx_off = 0; idx_off < 512; ++idx_off) {
		u64 i = (GB_frame_idx << 9) + idx_off;
		if (!megaframe_data[i].allocated)
			continue;
		switch (ps) {
		case PAGE_SIZE_4kB:
			if (megaframe_data[i].available_kiloframes != 0 && megaframe_data[i].available_kiloframes < minimum) {
				minimum = megaframe_data[i].available_kiloframes;
				found_idx = i;
				found_suitable = true;
			}
			break;
		case PAGE_SIZE_2MB: *found = i; return ERR_NONE;
		default:            return ERR_PHYSICAL_MEMORY_FULL;
		}
	}
	if (!found_suitable)
		return ERR_PHYSICAL_MEMORY_FULL;
	*found = found_idx;
	return ERR_NONE;
}

static u64 scan_for_suitable_kB_frame(u64 GB_frame_idx, u64 MB_frame_idx) {
	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	for (u64 idx_off = 0; idx_off < 512; ++idx_off) {
		u64 i = (GB_frame_idx << 18) + (MB_frame_idx << 9) + idx_off;
		if (!read_bitmap(i, kiloframe_bitmap))
			return i;
	}
	return -1; // If this happens a cosmic ray hit the computer.
}

//==================================
// Public
//==================================

error_t phys_mem_init(phys_buffer_t busy_regions) {
#ifdef __DEBUG__
	if (s_is_init)
		return ERR_NOT_VALID;
#endif
	ram_map_data_t ram_data = {0};
	error_t err = ram_map_get_data(&ram_data);
	if (err)
		return ERR_INTERNAL_FAILURE;
	size_t kiloframe_bitmap_size = ram_data.size_GB << 18; // ram size in GB * 2^30 / 2^12 (4kB)
	size_t megaframe_data_size = (ram_data.size_GB << 9);
	size_t gigaframe_data_size = (ram_data.size_GB);
	size_t alloc_size = kiloframe_bitmap_size + (megaframe_data_size * sizeof(megaframe_data_t)) +
	                    (gigaframe_data_size * sizeof(gigaframe_data_t));
	phys_mem_region_t mem_reg = {.size = alloc_size, .addr = 0};
	err = phys_mem_find_free_region(0x1000, busy_regions, &mem_reg);
	if (err)
		return ERR_INTERNAL_FAILURE;
	s_kiloframe_bitmap = mem_reg.addr;
	s_megaframe_data = s_kiloframe_bitmap + kiloframe_bitmap_size;
	s_gigaframe_data = s_megaframe_data + megaframe_data_size;

	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	memset(kiloframe_bitmap, 0, kiloframe_bitmap_size);
	for (u64 i = 0; i < megaframe_data_size; ++i)
		megaframe_data[i] = (megaframe_data_t){.available_kiloframes = (1ull << 9), .allocated = false};
	for (u64 i = 0; i < gigaframe_data_size; ++i) {
		gigaframe_data[i] = (gigaframe_data_t){
		    .available_kiloframes = (1ull << 18), .available_megaframes = (1ull << 9), .allocated = false};
	}
	phys_mem_region_t busy_regions_data[busy_regions.count + 1];
	memcpy(busy_regions_data, busy_regions.regions, busy_regions.count * sizeof(phys_mem_region_t));
	busy_regions_data[busy_regions.count] = mem_reg;
	busy_regions.regions = busy_regions_data;
	++busy_regions.count;
	err = phys_mem_announce_busy_regions(busy_regions);
	if (err)
		return ERR_INTERNAL_FAILURE;
#ifdef __DEBUG__
	s_is_init = true;
#endif
	return ERR_NONE;
}

error_t phys_mem_announce_busy_regions(phys_buffer_t regions) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif
	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	ram_map_data_t ram_data = {0};
	error_t err = ram_map_get_data(&ram_data);
	if (err)
		return ERR_INTERNAL_FAILURE;
	for (u64 i = 0; i < regions.count; ++i) {
		ppn_t region_pn = (regions.regions[i].addr - ram_data.phys_addr) >> 12;
		size_t occupied_frames_count = (regions.regions[i].size >> 12) + ((regions.regions[i].size & 0x3ff) != 0);
		for (size_t ppn = region_pn; ppn < region_pn + occupied_frames_count; ++ppn) {
			set_bitmap(true, ppn, kiloframe_bitmap);
			--megaframe_data[ppn >> 9].available_kiloframes;
			--gigaframe_data[ppn >> 18].available_kiloframes;
			if (megaframe_data[ppn >> 9].available_kiloframes == 511)
				--gigaframe_data[ppn >> 18].available_megaframes;
		}
	}
	return ERR_NONE;
}

error_t phys_mem_alloc_frame(page_size_t ps, ppn_t* ppnOUT) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif

	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);

	u64 GB_idx = 0;
	error_t err = scan_for_suitable_GB_frame(ps, &GB_idx);
	if (err)
		return err;
	if (ps == PAGE_SIZE_1GB) {
		gigaframe_data[GB_idx].allocated = true;
		*ppnOUT = (GB_idx << 18);
		return ERR_NONE;
	}
	u64 MB_idx = 0;
	err = scan_for_suitable_MB_frame(ps, GB_idx, &MB_idx);
	if (err)
		return err;
	if (ps == PAGE_SIZE_2MB) {
		megaframe_data[MB_idx].allocated = true;
		--gigaframe_data[GB_idx].available_megaframes;
		*ppnOUT = (MB_idx << 9);
		return ERR_NONE;
	}
	u64 kB_idx = scan_for_suitable_kB_frame(GB_idx, MB_idx);
	--megaframe_data[MB_idx].available_kiloframes;
	--gigaframe_data[GB_idx].available_kiloframes;
	if (megaframe_data[MB_idx].available_kiloframes == 511)
		--gigaframe_data[GB_idx].available_megaframes;
	set_bitmap(true, kB_idx, kiloframe_bitmap);
	*ppnOUT = kB_idx;
	return ERR_NONE;
}

error_t phys_mem_free_frame(ppn_t ppn) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
	ram_map_data_t ram_data = {0};
	(void)ram_map_get_data(&ram_data);
	if ((ram_data.size_GB << 18) < ppn)
		return ERR_BAD_ARG;
#endif

	const u64 MB_idx = ppn >> 9;
	const u64 GB_idx = ppn >> 18;
	u64* kiloframe_bitmap = physical_to_effective(s_kiloframe_bitmap);
	megaframe_data_t* megaframe_data = physical_to_effective(s_megaframe_data);
	gigaframe_data_t* gigaframe_data = physical_to_effective(s_gigaframe_data);
	if (read_bitmap(ppn, kiloframe_bitmap)) {
		set_bitmap(false, ppn, kiloframe_bitmap);
		++megaframe_data[MB_idx].available_kiloframes;
		++gigaframe_data[GB_idx].available_kiloframes;
		if (megaframe_data[MB_idx].available_kiloframes == 512)
			++gigaframe_data[GB_idx].available_megaframes;
		return ERR_NONE;
	}
	if (megaframe_data[MB_idx].allocated) {
		megaframe_data[MB_idx].allocated = false;
		++gigaframe_data[GB_idx].available_megaframes;
		return ERR_NONE;
	}
	if (gigaframe_data[GB_idx].allocated) {
		gigaframe_data[GB_idx].allocated = false;
		return ERR_NONE;
	}
	return ERR_NOT_VALID;
}

error_t phys_mem_find_free_region(u64 alignment, phys_buffer_t busy_regions, phys_mem_region_t* regOUT) {
	phys_buffer_t reserved_regions = {0};
	// TODO: Read reserved regions from device tree
	u64 ram_size = 0; // TODO: Read ram size from device tree
	phys_buffer_t unavailable_regions[2] = {reserved_regions, busy_regions};
	phys_addr_t curr_region_start = 0;
	bool overlap = false;
	while (curr_region_start + regOUT->size < ram_size) {
		for (u32 buff_idx = 0; buff_idx < sizeof(unavailable_regions) / sizeof(unavailable_regions[0]); ++buff_idx) {
			for (size_t reg_idx = 0; reg_idx < unavailable_regions[buff_idx].count; ++reg_idx) {
				phys_addr_t unavailable_region_start = unavailable_regions[buff_idx].regions[reg_idx].addr;
				phys_addr_t unavailable_region_end =
				    unavailable_region_start + unavailable_regions[buff_idx].regions[reg_idx].size;
				phys_addr_t curr_region_end = curr_region_start + regOUT->size;
				if (MAX(curr_region_start, unavailable_region_start) < MIN(curr_region_end, unavailable_region_end)) {
					curr_region_start = unavailable_region_end;
					curr_region_start = (phys_addr_t)align_up((u64)curr_region_start, alignment);
					overlap = true;
					break;
				}
			}
			if (overlap)
				break;
		}
		if (!overlap) {
			regOUT->addr = curr_region_start;
			return ERR_NONE;
		}
	}
	return ERR_PHYSICAL_MEMORY_FULL;
}
