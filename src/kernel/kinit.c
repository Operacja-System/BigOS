#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "stdbigos/error.h"
#include "virtual_memory/ram_map.h"
#include "virtual_memory/vmm.h"

//TEST:
#include <stdbigos/csr.h>

#define PANIC(err) if(err) { DEBUG_PRINTF("PANIC - error: %s", get_error_msg(err)); for(;;){continue;} }

extern void kmain();

extern void trap_vector();

void handle_trap(uint64_t scause, uint64_t sepc, uint64_t stval) {
    DEBUG_PRINTF("Trap! scause=0x%lx, sepc=0x%lx, stval=0x%lx\n", scause, sepc, stval);
    for(;;);
}

[[noreturn]] void kinit(u64 kernel_adr, u64 kernel_size, u64 kernel_stack_adr, u64 kernel_stack_size, u64 ram_start, u64 ram_size) {
	DEBUG_PRINTF("kinit() run\r\n");
	//TEST:
	//CSR_WRITE(stvec, ((uintptr_t)trap_vector & ~0x3ULL));

	
	set_ram_map_address((void*)ram_start);
	error_t err = initialize_virtual_memory(VMS_SV_48);
	PANIC(err);
	asid_t kernel_asid = 0;
	err = kernel_asid = create_address_space(PAGE_SIZE_2MB, true, false, &kernel_asid);
	PANIC(err);
	//TODO: Here kernel maps in VM must be created
	//err = enable_virtual_memory(kernel_asid);
	PANIC(err);
	kmain();
	DEBUG_PRINTF("ERROR kmain should never return\n");
	for (;;) {continue;}
}
