#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "bootstrap_memory_services.h"
#include "bootstrap_page_table.h"
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
	const size_t kernel_size = (uintptr_t)_binary_kernel_end - (uintptr_t)_binary_kernel_start;
	u64 ram_start = 0x80000000; // TODO: Read from DT
	u64 ram_size_GB = 1;		// TODO: Read from DT
	u64 ram_size = ram_size_GB * 0x40000000;
	void* device_tree = (void*)(dt_ppn << 12) + ram_start;

	init_boot_page_table_managment(TARGET_VMS);
	//u16 asid_max = initialize_virtual_memory();
	set_ram_params((void*)ram_start, ram_size);

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

	phisical_memory_region_t busy_mem_regions[PAGE_SIZE_AMOUNT + 1] = {0};
	phisical_memory_region_t* kernel_pmr = &busy_mem_regions[0];
	error_t pmr_alloc_err = ERR_NONE;
	pmr_alloc_err = allocate_phisical_memory_region(device_tree, nullptr, 0, kernel_size, 0x200000, kernel_pmr);
	if(pmr_alloc_err != ERR_NONE) PANIC("Failed to find free space for kernel image");

	*heap_region =
		(region_t){.addr = heap_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*ident_region =
		(region_t){.addr = ram_start, .size = ram_size, .mapped = true, .map_address = ram_start, .ps = PAGE_SIZE_1GB};
	*stack_region =
		(region_t){.addr = stack_addr, .size = 32 * 0x200000, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*kernel_region =
		(region_t){.addr = text_addr, .size = kernel_size, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*ram_map_region = (region_t){
		.addr = ram_map_addr, .size = ram_size, .mapped = true, .map_address = ram_start, .ps = PAGE_SIZE_1GB};

	required_memory_space_t mem_reg = calc_required_memory_for_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	if(mem_reg.error) PANIC("calculation of required memory space for page table failed");

	for(u8 i = 0; i < PAGE_SIZE_AMOUNT; ++i) {
		if(mem_reg.require_page_amounts[i] != 0) {
			pmr_alloc_err = allocate_phisical_memory_region(device_tree, busy_mem_regions, 1 + i,
															mem_reg.require_page_amounts[i] * (0x1000 << (9 * i)),
															(0x1000 << (9 * i)), &busy_mem_regions[i + 1]);
			if(pmr_alloc_err != ERR_NONE) PANIC("Failed to find free space for page table");
		}
	}

	set_page_memory_regions(&busy_mem_regions[1]);
	ppn_t root_ppn = create_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	DEBUG_PRINTF("[✔] Bootstrap page table was created successfully\n");
	void* kernel_entry_point_addr = nullptr;
	const error_t elf_loading_err =
		load_elf_at_address(_binary_kernel_start, kernel_pmr->address, &kernel_entry_point_addr);
	if(elf_loading_err != ERR_NONE) PANIC("Failed to load kernel elf");
	((void (*)(void))kernel_entry_point_addr)(); // HACK:
	PANIC("Kernel returned to kboot (this should never happen)");
}
