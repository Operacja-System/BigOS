#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "bootstrap_page_table.h"
#include "bootstrap_memory_allocator.h"
#include "bootstrap_panic.h"
#include "virtual_memory/mm_common.h"

extern unsigned char _binary_kernel_start[];
extern unsigned char _binary_kernel_end[];
extern size_t _binary_kernel_size;

extern void __executable_start;
extern void _DYNAMIC;

[[noreturn]] extern void kinit(u64 ram_map, u16 asid_max_val /*, device tree*/);

// WARNING: THIS WILL ONLY WORK IF KERNEL USES 2MiB PAGES

static constexpr virtual_memory_scheme_t TARGET_VMS = VMS_SV_48;

[[noreturn]] void kbootstrap(u64 load_address, u64 load_size, u64 dt_ppn) {
	u64 ram_start = 0;	 // TODO: Read from DT
	u64 ram_size_GB = 0; // TODO: Read from DT
	u64 ram_size = ram_size_GB * 0x40000000;
	void* device_tree = (void*)(dt_ppn << 12) + ram_start;

	DEBUG_PRINTF("KBOOT\n");

	init_boot_page_table_managment(TARGET_VMS, (void*)ram_start);
	initialize_virtual_memory();

	region_t regions[5] = {0};
	region_t* stack_region = &regions[0];
	region_t* heap_region = &regions[1];
	region_t* kernel_region = &regions[2];
	region_t* ident_region = &regions[3];
	region_t* ram_map_region = &regions[4];

	u64 stack_addr = 0;
	u64 heap_addr = 0;
	u64 text_addr = 0;
	u64 ram_map_addr = 0;

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
	ram_map_addr = heap_addr - ram_size;

	phisical_memory_region_t kernel_pmr = {0};
	error_t pmr_alloc_err = allocate_phisical_memory_region(device_tree, nullptr, 0, _binary_kernel_size, &kernel_pmr);
	if(pmr_alloc_err) PANIC("Failed to find free space for kernel image");

	*heap_region = (region_t){.addr = heap_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0};
	*ident_region = (region_t){.addr = ram_start, .size = ram_size, .mapped = true, .map_address = ram_start};
	*stack_region = (region_t){.addr = stack_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0};
	*kernel_region = (region_t){.addr = text_addr, .size = _binary_kernel_size, .mapped = false, .map_address = 0};
	*ram_map_region = (region_t){.addr = ram_map_addr, .size = ram_size, .mapped = true, .map_address = ram_start};

	required_memory_space_t mem_reg = calc_required_memory_for_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	phisical_memory_region_t free_mem_region = {0};
	pmr_alloc_err = allocate_phisical_memory_region(device_tree, &kernel_pmr, 1, mem_reg.total_in_bytes, &free_mem_region);
	if(pmr_alloc_err) PANIC("Failed to find free space for page table");

	page_table_meta_t ptm = create_page_table(regions, sizeof(regions) / sizeof(regions[0]), free_mem_region.address);
}
