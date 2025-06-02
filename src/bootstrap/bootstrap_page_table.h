#ifndef __BOOTSTRAP_BOOTSTRAP_PAGE_TALBE_H__
#define __BOOTSTRAP_BOOTSTRAP_PAGE_TALBE_H__

#include <stdbigos/error.h>

#include "bootstrap_memory_services.h"
#include "virtual_memory/mm_common.h"

typedef struct {
	u64 addr;
	u64 size;
	bool mapped;
	u64 map_address;
	page_size_t ps;
} region_t;

typedef struct {
	u64 require_page_amounts[PAGE_SIZE_AMOUNT];
	error_t error;
} required_memory_space_t;

[[nodiscard]] u16 initialize_virtual_memory(virtual_memory_scheme_t vms);
[[nodiscard]] required_memory_space_t calc_required_memory_for_page_table(region_t* regions, u64 regions_amount);
void set_page_memory_regions(physical_memory_region_t mem_regions[5]);
[[nodiscard]] ppn_t create_page_table(region_t* regions, u64 regions_amount);
// This function is not implemented in the most robust way but it will be used only for debuging anyways
// It should be called with vpn = 0
void print_page_table(ppn_t root_ppn, u8 start_lvl, vpn_t vpn);

#endif
