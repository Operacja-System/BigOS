#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "bootstrap_memory_services.h"
#include "bootstrap_page_table.h"
#include "bootstrap_panic.h"
#include "virtual_memory/mm_common.h"

extern unsigned char _binary_kernel_start[];
extern unsigned char _binary_kernel_end[];

[[noreturn]] extern void kinit(u64 ram_map, u16 asid_max_val /*, device tree*/);

static constexpr virtual_memory_scheme_t TARGET_VMS = VMS_SV_48;

[[noreturn]] void kbootstrap(u64 load_address, u64 load_size, u64 dt_ppn) {
	const size_t kernel_size = (uintptr_t)_binary_kernel_end - (uintptr_t)_binary_kernel_start;
	u64 ram_start = 0x80000000; // TODO: Read from DT
	u64 ram_size_in_GiB = 1;	// TODO: Read from DT
	u64 ram_size = ram_size_in_GiB * GiB;
	void* device_tree = (void*)(dt_ppn << 12) + ram_start;

	u16 asid_max = initialize_virtual_memory(TARGET_VMS);
	set_ram_params((void*)ram_start, ram_size);

	region_t regions[5] = {0};
	region_t* stack_region = &regions[0];
	region_t* heap_region = &regions[1];
	region_t* kernel_region = &regions[2];
	region_t* ident_region = &regions[3];
	region_t* ram_map_region = &regions[4];

	u64 stack_size_in_bytes = 32 * MiB;
	u64 heap_size_in_bytes = 32 * MiB;
	u64 stack_top_addr = (1ull << (39 + 9 * TARGET_VMS)) - GiB - stack_size_in_bytes;
	u64 heap_addr = 3ull << (39 + 9 * TARGET_VMS - 2);
	u64 text_addr = 1ull << (39 + 9 * TARGET_VMS - 1);
	u64 ram_map_addr = heap_addr - ram_size;

	physical_memory_region_t busy_mem_regions[PAGE_SIZE_AMOUNT + 1] = {0};
	physical_memory_region_t* kernel_pmr = &busy_mem_regions[0];
	error_t pmr_alloc_err = ERR_NONE;
	pmr_alloc_err = allocate_physical_memory_region(device_tree, kernel_size, 2 * MiB, kernel_pmr);
	if(pmr_alloc_err != ERR_NONE) PANIC("Failed to find free space for kernel image");
	DEBUG_PRINTF("[i] Kernel region at: 0x%lx\n", (u64)kernel_pmr->address);

	*heap_region = (region_t){
		.addr = heap_addr, .size = heap_size_in_bytes, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*ident_region =
		(region_t){.addr = ram_start, .size = ram_size, .mapped = true, .map_address = ram_start, .ps = PAGE_SIZE_1GB};
	*stack_region = (region_t){
		.addr = stack_top_addr, .size = stack_size_in_bytes, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*kernel_region =
		(region_t){.addr = text_addr, .size = kernel_size, .mapped = false, .map_address = 0, .ps = PAGE_SIZE_2MB};
	*ram_map_region = (region_t){
		.addr = ram_map_addr, .size = ram_size, .mapped = true, .map_address = ram_start, .ps = PAGE_SIZE_1GB};

	required_memory_space_t mem_reg = calc_required_memory_for_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	if(mem_reg.error) PANIC("calculation of required memory space for page table failed");

	for(u8 i = 0; i < PAGE_SIZE_AMOUNT; ++i) {
		const u64 page_size_in_bytes = ((4 * kiB) << (9 * i));
		if(mem_reg.require_page_amounts[i] != 0) {
			const u64 size_in_bytes = mem_reg.require_page_amounts[i] * page_size_in_bytes;
			physical_memory_region_t* target_pmr = &busy_mem_regions[i + 1];
			pmr_alloc_err = allocate_physical_memory_region(device_tree, size_in_bytes, page_size_in_bytes, target_pmr);
			if(pmr_alloc_err != ERR_NONE) PANIC("Failed to find free space for page table");
		}
	}
	set_page_memory_regions(&busy_mem_regions[1]);

	ppn_t root_ppn = create_page_table(regions, sizeof(regions) / sizeof(regions[0]));
	DEBUG_PRINTF("[✔] Bootstrap page table was created successfully\n");

	void* kernel_entry_point_addr = nullptr;
	const error_t elf_loading_err =
		load_elf_at_address(_binary_kernel_start, kernel_pmr->address, &kernel_entry_point_addr);
	if(elf_loading_err != ERR_NONE) PANIC("Failed to load kernel ELF");
	DEBUG_PRINTF("[✔] Kernel ELF loaded successfully\n");

	for(;;);
	((void (*)(void))kernel_entry_point_addr)(); // HACK:
	PANIC("Kernel returned to kboot (this should never happen)");
}
