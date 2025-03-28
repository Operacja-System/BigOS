#include <debug/debug_stdio.h>
#include <stdbigos/types.h>

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

volatile static u8 tabeleczka[4194304] = {0};

void kinit(u64 kernel_physical_address, u64 kernel_size, u64 stack_physical_address, u64 stack_size) {
	DEBUG_PRINTF("Kernel loaded at phisical address: 0x%lx (size: %luMB). Stack at: 0x%lx (size: %luMB)\n", kernel_physical_address, kernel_size >> 20, stack_physical_address, stack_size >> 20);
}
