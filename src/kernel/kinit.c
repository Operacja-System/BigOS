#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

#include "stdbigos/error.h"
#include "virtual_memory/vmm.h"

/* Phisical memory ('|' - 2MB)
 *  _________
 * |         |
 * |         |
 * |         |
 * |         |
 * |         |
 * |         |
 * |         |
 * |         |
 *	   ...
 * |         |
 * |         | - Kernel stack
 * |         | - Kernel
 * |_________| - Kernel
 */

// NOTE: This is an example so far.
[[noreturn]] void kinit(u64 kernel_physical_address, u64 kernel_size, u64 stack_physical_address, u64 stack_size,
						void* ram_start) {
	DEBUG_PRINTF("Kernel loaded at phisical address: 0x%lx (size: %luMB). Stack at: 0x%lx (size: %luMB)\n",
				 kernel_physical_address, kernel_size >> 20, stack_physical_address, stack_size >> 20);
	error_t err = virtual_memory_init(ram_start);
	err = virtual_memory_enable(VMS_BARE, 0);
	if(err)
		DEBUG_PRINTF("Error: %s\n", get_error_msg(err));
	else
		DEBUG_PRINTF("Virtual memory enabled (vms: %s), asidlen: %hu\n",
					 get_virt_mem_scheme_str_name(get_active_virt_mem_scheme()), get_asid_max_val());

	for(;;);
}
