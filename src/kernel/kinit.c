#include <debug/debug_stdio.h>

#include "kernel_config.h"
#include "klog.h"
#include "memory_managment/address_space_manager.h"
#include "memory_managment/mm_types.h"
#include "memory_managment/physical_memory_manager.h"
#include "memory_managment/virtual_memory_managment.h"
#include "power.h"
#include "ram_map.h"

#define IF_ANY_ERR_LOG_AND_PANIC(err, log, ...)                                         \
	do {                                                                                \
		if (err) {                                                                      \
			KLOGLN_ERROR(log " failed with error: %u", __VA_ARGS__ __VA_OPT__(, ) err); \
			halt(); /*TODO: This should be a panic*/                                    \
		}                                                                               \
	} while (0)

[[noreturn]] extern void kmain();

[[noreturn]] void kinit([[maybe_unused]] volatile u64 boot_hartid, volatile phys_addr_t device_tree) {
	KLOGLN_NOTE("Begining kernel initialization");

	kernel_config_t kercfg = {0};
	phys_addr_t ram_start = 0x80000000;                            // TODO: Read from DT
	size_t ram_size = (1ull << 30);                                // TODO: Read from DT
	kerconf_cpu_endian_t endianness = KC_CPU_ENDIAN_LITTLE_ENDIAN; // TODO: Read from DT
	kercfg.device_tree_phys_addr = device_tree;
	kercfg.cpu_endian = endianness;
	kercfg.machine = KC_MACHINE_RISCV;                             // TODO: Read from DT?
	kercfg.mode = 64;                                              // TODO: Read from DT?
	kercfg.target_vms = KC_VMS_SV48; // TODO: I think this would make sense to be passed as kernel arg
	kercfg.active_vms = KC_VMS_BARE;
	error_t err = kernel_config_set(kercfg);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Kernel config initialization");
	kernel_config_log();

	ram_map_data_t ram_data = {0};
	ram_data.size = ram_size;
	ram_data.addr = (void*)ram_start;
	ram_data.phys_addr = ram_start;
	ram_map_set_data(ram_data);

	// NOTE: if any regions are allocated in phys memory before initializing phys_mem_mgr they need to be added to this
	// buffer
	phys_mem_region_t kreg = {.addr = 0x80000000, .size = 0x200000};
	phys_buffer_t phys_buffer = (phys_buffer_t){.count = 1, .regions = &kreg};
	err = phys_mem_init(phys_buffer);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Physical memory manager initialization");
	KLOGLN_NOTE("Phisical memory manager initialized");

	u16 max_asid = virt_mem_get_max_asid();
	err = address_space_managment_init(max_asid);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Address space manager initialization");
	KLOGLN_NOTE("Address space manager initialized");

	u8 as_bits = 0;
	buffer_t as_bits_buffer = kernel_config_get(KERCFG_ADDRESS_SPACE_BITS);
	const bool buffer_ok = buffer_read_u8(as_bits_buffer, 0, &as_bits);
	IF_ANY_ERR_LOG_AND_PANIC(!buffer_ok, "Reading kernel config");

	const size_t stack_size = 6 * (1ull << 20);
	const size_t mstack_size = 2 * (1ull << 20);                          // TODO: Read from dt
	const size_t heap_size = 8 * (1ull << 20);

	void* mstack_bottom_addr = (void*)((1ull << as_bits) - (1ull << 30)); // NOTE: 1GB buffer
	void* mstack_top_addr = mstack_bottom_addr - mstack_size;
	void* stack_bottom_addr = mstack_top_addr;
	void* stack_top_addr = stack_bottom_addr - stack_size;
	void* heap_addr = (void*)(3ull << (as_bits - 2));
	void* text_addr = (void*)(1ull << (as_bits - 1)) + (1ull << 30); // NOTE: 1GB buffer
	void* ram_map_addr = heap_addr - ram_data.size;

	const phys_addr_t stack_phys_addr = 0;                           // TODO: Read from DT
	const phys_addr_t text_phys_addr = 0xbde80b70;                   // TODO: Read from DT
	const size_t text_size = 2 * (1ull << 20);                       // TODO: Read from DT
	const phys_mem_region_t ram_map_phys_region = {.size = ram_data.size, .addr = ram_data.phys_addr};
	const phys_mem_region_t stack_phys_region = {.size = mstack_size, .addr = stack_phys_addr};
	const phys_mem_region_t text_phys_region = {.size = text_size, .addr = text_phys_addr};
	const phys_mem_region_t physical_address_space = {.size = ram_data.phys_addr + ram_data.size, .addr = 0};

	const virt_mem_region_t kernel_address_space_regions[] = {
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
	                        .debug_comment = "Phys mem stack map",
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
	                        .debug_comment = "Virt mem stack region",
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
	                        .debug_comment = "Virt mem heap region",
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
	                        .debug_comment = "Text region",
	                        },
	    (virt_mem_region_t){
	                        .addr = ram_map_addr,
	                        .size = ram_data.size,
	                        .mapped = true,
	                        .map_region = ram_map_phys_region,
	                        .ps = PAGE_SIZE_1GB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = true,
	                        .debug_comment = "Virt mem ram map",
	                        },
	    (virt_mem_region_t){
	                        .addr = (void*)0x0,
	                        .size = ram_data.phys_addr + ram_data.size,
	                        .mapped = true,
	                        .map_region = physical_address_space,
	                        .ps = PAGE_SIZE_1GB,
	                        .global = true,
	                        .user = false,
	                        .read = true,
	                        .write = true,
	                        .execute = true,
	                        .debug_comment = "Identity mapping",
	                        },
	};

	as_handle_t kernel_ash = {0};
	err = address_space_create(&kernel_ash, false, true);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Creation of kernel address space");

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
		error_t err = address_space_add_region(&kernel_ash, kernel_address_space_regions[i]);
		IF_ANY_ERR_LOG_AND_PANIC(err, "Adding virtual memory region #%u", i);
	}
	KLOGLN_NOTE("Mapped kernel memory regions.");

	err = address_space_set_stack_data(&kernel_ash, stack_bottom_addr, stack_size);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Updating kernel address space");
	err = address_space_set_heap_data(&kernel_ash, heap_addr, heap_size);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Updating kernel address space");

	KLOGLN_NOTE("Kernel initialization complete. Enabling virtual memory...");
	err = address_space_set_active(&kernel_ash);
	IF_ANY_ERR_LOG_AND_PANIC(err, "Setting kernel address space as active");
	KLOGLN_NOTE("Virtual memory online. Jumping to kernel...");

	// TODO: Kernel should jump to a higher address space before jumping to main
	// TODO: Kernel identy mapping should be deleted here (after this /\)

	kmain();

	KLOGLN_ERROR("kmain returned. This should never happen!");
	halt(); // TODO: Panic
}
