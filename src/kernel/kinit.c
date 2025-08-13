#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "stdbigos/error.h"
#include "virtual_memory/pmm.h"
#include "virtual_memory/ram_map.h"
#include "virtual_memory/vmm.h"

#define PANIC(err)                                             \
	if(err) {                                                  \
		DEBUG_PRINTF("PANIC - error: %s", get_error_msg(err)); \
		for(;;) { continue; }                                  \
	}
	kernel_config_log();

// TEST:
#include <stdbigos/csr.h>
extern void trap_vector();
void handle_trap(uint64_t scause, uint64_t sepc, uint64_t stval) {
	DEBUG_PRINTF("Trap! scause=0x%lx, sepc=0x%lx, stval=0x%lx\n", scause, sepc, stval);
	for(;;);
}

extern void kmain();

[[gnu::naked]] void _start() {
	asm("j kinit");
}

[[noreturn]] void kinit(u64 ram_map, u16 asid_max_val /*, device tree*/) {
	u64 ram_size = 1; // HACK: this will be later retrieved from device tree
	DEBUG_PRINTF("kinit() run. ram_size: %lu\r\n", ram_size);
	// TEST:
	// CSR_WRITE(stvec, ((uintptr_t)trap_vector & ~0x3ULL));

	error_t err = initialize_pmm(VMS_SV_48, ram_size);
	PANIC(err);
	set_ram_map_address((void*)ram_map);
	asid_t kernel_asid = 0;
	err = kernel_asid = create_address_space(PAGE_SIZE_2MB, true, false, &kernel_asid);
	PANIC(err);
	// TODO: Here kernel maps in VM must be created

	// err = enable_virtual_memory(kernel_asid);
	PANIC(err);
	kmain();
	DEBUG_PRINTF("ERROR kmain should never return\n");
	for(;;) { continue; }
}
