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
	bool error;
} required_memory_space_t;

void init_boot_page_table_managment(virtual_memory_scheme_t vms);
void set_page_memory_regions(phisical_memory_region_t mem_regions[5]);
[[nodiscard]] required_memory_space_t calc_required_memory_for_page_table(region_t* regions, u64 regions_amount);
[[nodiscard]] u16 initialize_virtual_memory();
[[nodiscard]] ppn_t get_page_frame(page_size_t ps);
[[nodiscard]] ppn_t create_page_table(region_t* regions, u64 regions_amount);

#endif
