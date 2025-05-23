#include <stdbigos/types.h>

#include "bootstrap_page_table.h"
#include "virtual_memory/mm_common.h"

extern unsigned char _binary_kernel_start[];
extern unsigned char _binary_kernel_start[];
extern size_t _binary_kernel_size;

[[noreturn]] extern void kinit(u64 ram_map, u16 asid_max_val /*, device tree*/);

// WARNING: THIS WILL ONLY WORK IF KERNEL USES 2MiB PAGES

static constexpr virtual_memory_scheme_t TARGET_VMS = VMS_SV_48;

[[noreturn]] void kbootstrap(u64 load_address, u64 load_size, u64 dt_ppn) {
	u64 ram_start = 0;	 // TODO: Read from DT
	u64 ram_size_GB = 0; // TODO: Read from DT
	u64 ram_size = ram_size_GB * 0x40000000;

	init_boot_page_table_managment(TARGET_VMS, (void*)ram_start);
	initialize_virtual_memory();

	region_t regions[5] = {0};
	region_t* stack_region = &regions[0];
	region_t* heap_region = &regions[1];
	region_t* text_region = &regions[2];
	region_t* ident_region = &regions[3];
	region_t* ram_map_region = &regions[4];

	u64 stack_addr = 0;
	u64 heap_addr = 0;
	u64 text_addr = 0;

	switch(TARGET_VMS) {
	case VMS_SV_39:
		stack_addr = (1ull << 39) - (1ull << 30);
		heap_addr = (3ull << 37);
		text_addr = (1ull << 38);
		break;
	case VMS_SV_48:
		stack_addr = (1ull << 48) - (1ull << 30);
		heap_addr = (3ull << 46);
		text_addr = (1ull << 47);
		break;
	case VMS_SV_57:
		stack_addr = (1ull << 57) - (1ull << 30);
		heap_addr = (3ull << 55);
		text_addr = (1ull << 56);
		break;
	}

	*stack_region = (region_t){.addr = stack_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0};
	*heap_region = (region_t){.addr = heap_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0};
	*text_region = (region_t){.addr = text_addr, .size = load_size, .mapped = true, .map_address = load_address};
	*ident_region = (region_t){.addr = load_address, .size = load_size, .mapped = true, .map_address = load_address};
	*ram_map_region =
		(region_t){.addr = heap_addr - ram_size, .size = ram_size, .mapped = true, .map_address = ram_start};

	required_memory_space_t mem_reg = calc_required_memory_for_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	void* free_mem_region = nullptr; // TODO: find

	page_table_meta_t ptm = create_page_table(regions, sizeof(regions) / sizeof(regions[0]), free_mem_region);
}
