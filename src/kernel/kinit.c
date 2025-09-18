#include <debug/debug_stdio.h>

#include "kernel_config.h"
#include "klog.h"
#include "memory_managment/address_space_manager.h"
#include "memory_managment/mm_types.h"
#include "memory_managment/physical_memory_manager.h"
#include "memory_managment/virtual_memory_managment.h"
#include "power.h"
#include "ram_map.h"

[[noreturn]] extern void kmain();

[[noreturn]] void kinit([[maybe_unused]] volatile u64 boot_hartid, volatile phys_addr_t device_tree) {
	void* pc = nullptr;
	asm volatile("auipc %0, 0" : "=r"(pc));
	KLOGLN_TRACE("pc is: %p", pc);
	KLOGLN_NOTE("Begining kernel initialization");

	kernel_config_t kercfg = {0};
	phys_addr_t ram_start = 0x80000000; // TODO: Read from DT
	//						  10000005
	//					    0x10000000
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
		halt();
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
	phys_mem_region_t kreg = {.addr = 0x80000000, .size = 0x200000};
	phys_buffer_t phys_buffer = (phys_buffer_t){.count = 1, .regions = &kreg};
	err = phys_mem_init(phys_buffer);
	if (err) {
		KLOGLN_ERROR("Physical memory manager initialization failed with error %u", err);
		halt();
		/*TODO: Panic*/
	}
	KLOGLN_NOTE("Phisical memory manager initialized");

	u16 max_asid = virt_mem_get_max_asid();
	err = address_space_managment_init(max_asid);
	if (err) {
		KLOGLN_ERROR("Address space manager initialization failed with error %u", err);
		halt();
		/*TODO: Panic*/
	}
	KLOGLN_NOTE("Address space manager initialized");

	u8 as_bits = 0;
	buffer_t as_bits_buffer = kernel_config_get(KERCFG_ADDRESS_SPACE_BITS);
	if (as_bits_buffer.error) {
		KLOGLN_ERROR("Reading kernel config failed with error %u", err);
		halt();
		/*TODO: Panic*/
	}
	err = buffer_read_u8(as_bits_buffer, 0, &as_bits);
	if (err) {
		KLOGLN_ERROR("Reading kernel config failed with error %u", err);
		halt();
		/*TODO: Panic*/
	}

	size_t stack_size = 8 * (1ull << 20);
	size_t mstack_size = 2 * (1ull << 21);                                // TODO: Read from dt
	size_t heap_size = 8 * (1ull << 20);

	void* mstack_bottom_addr = (void*)((1ull << as_bits) - (1ull << 30)); // NOTE: 1GB buffer
	void* mstack_top_addr = mstack_bottom_addr - mstack_size;
	void* stack_bottom_addr = mstack_top_addr;
	void* stack_top_addr = stack_bottom_addr - stack_size;
	void* heap_addr = (void*)(3ull << (as_bits - 2));
	void* text_addr = (void*)(1ull << (as_bits - 1)) + (1ull << 30); // NOTE: 1GB buffer
	void* ram_map_addr = heap_addr - (ram_data.size_GB << 30);

	phys_addr_t stack_phys_addr = 0;                                 // TODO: Read from DT
	phys_addr_t text_phys_addr = 0xbde80b70;                         // TODO: Read from DT
	size_t text_size = 2 * (1ull << 20);                             // TODO: Read from DT
	const phys_mem_region_t ram_map_phys_region = {.size = ram_data.size_GB << 30, .addr = ram_data.phys_addr};
	const phys_mem_region_t stack_phys_region = {.size = mstack_size, .addr = stack_phys_addr};
	const phys_mem_region_t text_phys_region = {.size = text_size, .addr = text_phys_addr};
	const phys_mem_region_t physical_address_space = {.size = ram_data.phys_addr + (ram_data.size_GB << 30), .addr = 0};

	virt_mem_region_t kernel_address_space_regions[] = {
	    (virt_mem_region_t){
	                        .addr = mstack_top_addr,
	                        .size = mstack_size,
	                        .mapped = true,
	                        .map_region = stack_phys_region,
	                        .ps = PAGE_SIZE_2MB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = false,
#ifdef __DEBUG__
	                        .debug_comment = "Phys mem stack map",
                            #endif
	                        },
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
#ifdef __DEBUG__
	                        .debug_comment = "Virt mem stack region",
                            #endif
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
#ifdef __DEBUG__
	                        .debug_comment = "Virt mem heap region",
                            #endif
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
#ifdef __DEBUG__
	                        .debug_comment = "Text region",
                            #endif
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
	                        .execute = true,
#ifdef __DEBUG__
	                        .debug_comment = "Virt mem ram map",
                            #endif
	                        },
	    (virt_mem_region_t){
	                        .addr = (void*)0x0,
	                        .size = ram_data.phys_addr + (ram_data.size_GB << 30),
	                        .mapped = true,
	                        .map_region = physical_address_space,
	                        .ps = PAGE_SIZE_1GB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = true,
#ifdef __DEBUG__
	                        .debug_comment = "Identity mapping",
                            #endif
	                        },
	};

	as_handle_t kernel_ash = {0};
	err = address_space_create(&kernel_ash, false, true);
	if (err) {
		KLOGLN_ERROR("Failed to create kernel address space with error %u", err);
		halt();
		/*TODO: Panic*/
	}

	KLOGLN_NOTE("Created kernel address space");
	for (u32 i = 0; i < sizeof(kernel_address_space_regions) / sizeof(kernel_address_space_regions[0]); ++i) {
		KLOGLN_NOTE("Kernel ram map (VMR#%u):", i);
		KLOG_INDENT_BLOCK_START;
		log_virt_mem_region(&kernel_address_space_regions[i]);
		KLOG_INDENT_BLOCK_END;
	}

	for (u32 i = 0; i < sizeof(kernel_address_space_regions) / sizeof(kernel_address_space_regions[0]); ++i) {
		u64 kasr_addr = (u64)kernel_address_space_regions[i].addr;
		u64 kasr_size = (u64)kernel_address_space_regions[i].size;
		KLOGLN_TRACE("Adding VMR#%u to kernel address space. Addr range: %lx - %lx", i, kasr_addr,
		             kasr_addr + kasr_size);
		error_t err = address_sapce_add_region(&kernel_ash, kernel_address_space_regions[i]);
		if (err) {
			KLOGLN_ERROR("Failed to add virtual memory region #%u to kernel address space with error %u", i, err);
			halt();
			/*TODO: Panic*/
		}
	}
	KLOGLN_NOTE("Mapped kernel memory regions.");

	err = address_space_set_stack_data(&kernel_ash, stack_bottom_addr, stack_size);
	if (err) {
		KLOGLN_ERROR("Failed to update kernel address space data with error %u", err);
		halt();
		/*TODO: Panic*/
	}
	err = address_space_set_heap_data(&kernel_ash, heap_addr, heap_size);
	if (err) {
		KLOGLN_ERROR("Failed to update kernel address space data with error %u", err);
		halt();
		/*TODO: Panic*/
	}

	// TEST:
	{
		address_space_print_page_table(&kernel_ash);
		phys_addr_t pa = 0;
		err = address_space_vaddr_to_paddr(&kernel_ash, (void*)0x10000000, &pa);
		if (err)
			KLOGLN_ERROR("err: %u", err);
		else
			KLOGLN_NOTE("vaddr: %p -> paddr: %lx", (void*)0x10000000, pa);
	}

	KLOGLN_NOTE("Kernel initialization complete. Jumping to kernel...");
	virt_mem_set_satp(max_asid, kercfg.target_vms, kernel_ash.root_pte);
	virt_mem_flush_TLB();
	KLOGLN_NOTE("VMEM online");
	kmain();

	KLOGLN_ERROR("kmain returned. This should never happen");
	halt();
}
