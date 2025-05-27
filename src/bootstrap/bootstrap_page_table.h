#ifndef __KERNEL_BOOTSTRAP_BOOTSTRAP_PAGE_TALBE_H__
#define __KERNEL_BOOTSTRAP_BOOTSTRAP_PAGE_TALBE_H__

#include "virtual_memory/mm_common.h"

typedef struct {
	u64 addr;
	u64 size;
	bool mapped;
	u64 map_address;
} region_t;

typedef struct {
	u64 amount_of_4kb_pages;
	u64 amount_of_2Mb_pages;
	u64 total_in_bytes;
	bool error;
} required_memory_space_t;

typedef struct {
	ppn_t root_pt_ppn;
	u64 used_4kB_pages;
	u64 used_2MB_pages;
} page_table_meta_t;

void init_boot_page_table_managment(virtual_memory_scheme_t vms, void* ram_start);
required_memory_space_t calc_required_memory_for_page_table(region_t regions[], u64 regions_amount);
u16 initialize_virtual_memory();
ppn_t get_page_frame(page_size_t ps);
page_table_meta_t create_page_table(region_t regions[], u64 regions_amount, void* mem_region);

#endif
