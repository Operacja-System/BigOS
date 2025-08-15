#include <debug/debug_stdio.h>

#include "kernel_config.h"
#include "memory_managment/address_space_manager.h"
#include "memory_managment/mm_types.h"
#include "memory_managment/page_table.h"
#include "memory_managment/physical_memory_manager.h"
#include "memory_managment/virtual_memory_managment.h"
#include "ram_map.h"
#include "klog.h"

[[noreturn]] extern void kmain();

[[noreturn]] void kinit([[maybe_unused]] volatile u64 boot_hartid, volatile phys_addr_t device_tree) {
	KLOGLN_NOTE("Begining kernel initialization");

	kernel_config_t kercfg = {0};
	phys_addr_t ram_start = 0x80200000;                            // TODO: Read from DT
	size_t ram_size = 1;                                           // TODO: Read from DT
	kerconf_cpu_endian_t endianness = KC_CPU_ENDIAN_LITTLE_ENDIAN; // TODO: Read from DT
	kercfg.device_tree_phys_addr = device_tree;
	kercfg.cpu_endian = endianness;
	kercfg.machine = KC_MACHINE_RISCV;                             // TODO: Read from DT?
	kercfg.mode = 64;                                              // TODO: Read from DT
	kercfg.target_vms = KC_VMS_SV48;
	kercfg.active_vms = KC_VMS_BARE;
	error_t err = kernel_config_set(kercfg);
	if (err) {
		KLOGLN_ERROR("Kernel config initialization failed with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	kernel_config_log();

	ram_map_data_t ram_data = {0};
	ram_data.size_GB = ram_size;
	ram_data.addr = (void*)ram_start;
	ram_data.phys_addr = ram_start;
	ram_map_set_data(ram_data);

	// NOTE: if any regions are allocated in phys memory before initializing phys_mem_mgr they need to be added to this
	// buffer
	phys_buffer_t phys_buffer = (phys_buffer_t){.count = 0, .regions = nullptr};
	err = phys_mem_init(phys_buffer);
	if (err) {
		KLOGLN_ERROR("Physical memory manager initialization failed with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	KLOGLN_NOTE("Phisical memory manager initialized");

	u16 max_asid = virt_mem_get_max_asid();
	err = address_space_managment_init(max_asid);
	if (err) {
		KLOGLN_ERROR("Address space manager initialization failed with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	KLOGLN_NOTE("Address space manager initialized");

	u8 as_bits = 0;
	buffer_t as_bits_buffer = kernel_config_get(KERCFG_ADDRESS_SPACE_BITS);
	if (as_bits_buffer.error) {
		KLOGLN_ERROR("Reading kernel config failed with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	err = buffer_read_u8(as_bits_buffer, 0, &as_bits);
	if (err) {
		KLOGLN_ERROR("Reading kernel config failed with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}

	size_t stack_size = 8 * (1ull << 20);
	size_t heap_size = 8 * (1ull << 20);

	void* stack_bottom_addr = (void*)((1ull << as_bits) - (1ull << 30));
	void* stack_top_addr = stack_bottom_addr - stack_size;
	void* heap_addr = (void*)(3ull << (as_bits - 2));
	void* text_addr = (void*)(1ull << (as_bits - 1));
	void* ram_map_addr = heap_addr - (ram_data.size_GB << 18);

	phys_addr_t text_phys_addr = 0x80000000; // TODO: Read from DT
	size_t text_size = 2 * (1ull << 20);     // TODO: Read from DT
	phys_mem_region_t ram_map_phys_region =
	    (phys_mem_region_t){.size = ram_data.size_GB << 18, .addr = ram_data.phys_addr};
	phys_mem_region_t text_phys_region = (phys_mem_region_t){.size = text_size, .addr = text_phys_addr};

	virt_mem_region_t kernel_address_space_regions[4] = {
	    (virt_mem_region_t){
	                        .addr = stack_top_addr,
	                        .size = stack_size,
	                        .mapped = false,
	                        .map_region = {0},
	                        .ps = PAGE_SIZE_2MB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = false,
	                        },
	    (virt_mem_region_t){
	                        .addr = heap_addr,
	                        .size = heap_size,
	                        .mapped = false,
	                        .map_region = {0},
	                        .ps = PAGE_SIZE_2MB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = false,
	                        },
	    (virt_mem_region_t){
	                        .addr = text_addr,
	                        .size = text_size,
	                        .mapped = true,
	                        .map_region = text_phys_region,
	                        .ps = PAGE_SIZE_2MB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = false,
	                        .execute = true,
	                        },
	    (virt_mem_region_t){
	                        .addr = ram_map_addr,
	                        .size = ram_data.size_GB << 30,
	                        .mapped = true,
	                        .map_region = ram_map_phys_region,
	                        .ps = PAGE_SIZE_1GB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = false,
	                        },
	};

	as_handle_t kernel_ash = {0};
	err = address_space_create(&kernel_ash, false, true);
	if (err) {
		KLOGLN_ERROR("Failed to create kernel address space with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	KLOGLN_NOTE("Created kernel address space");

	KLOGLN_NOTE("Kernel stack (VMR#0):");
	KLOG_INDENT_BLOCK_START;
	log_virt_mem_region(kernel_address_space_regions[0]);
	KLOG_INDENT_BLOCK_END;
	KLOGLN_NOTE("Kernel heap (VMR#1):");
	KLOG_INDENT_BLOCK_START;
	log_virt_mem_region(kernel_address_space_regions[1]);
	KLOG_INDENT_BLOCK_END;
	KLOGLN_NOTE("Kernel text (VMR#2):");
	KLOG_INDENT_BLOCK_START;
	log_virt_mem_region(kernel_address_space_regions[2]);
	KLOG_INDENT_BLOCK_END;
	KLOGLN_NOTE("Kernel ram map (VMR#3):");
	KLOG_INDENT_BLOCK_START;
	log_virt_mem_region(kernel_address_space_regions[3]);
	KLOG_INDENT_BLOCK_END;

	for (u32 i = 0; i < sizeof(kernel_address_space_regions) / sizeof(kernel_address_space_regions[0]); ++i) {
		KLOGLN_TRACE("Adding VMR#%u to kernel address space.", i);
		error_t err = address_sapce_add_region(&kernel_ash, kernel_address_space_regions[i]);
		if (err) {
			KLOGLN_ERROR("Failed to add virtual memory region #%u to kernel address space with error %u", i, err);
			for (;;);
			/*TODO: Panic*/
		}
	}
	KLOGLN_NOTE("Mapped kernel memory regions.");

	err = address_space_set_stack_data(&kernel_ash, stack_bottom_addr, stack_size);
	if (err) {
		KLOGLN_ERROR("Failed to update kernel address space data with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}
	err = address_space_set_heap_data(&kernel_ash, heap_addr, heap_size);
	if (err) {
		KLOGLN_ERROR("Failed to update kernel address space data with error %u", err);
		for (;;);
		/*TODO: Panic*/
	}

	KLOGLN_NOTE("Kernel initialization complete. Jumping to kernel...");

	// NOTE: As it is done now between kinit and kmain the stack will change.
	kmain();

	KLOGLN_ERROR("kmain returned. This should never happen");
	for (;;);
}
