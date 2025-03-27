#include <debug/debug_stdio.h>
#include <stdint.h>

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

void kinit(uint64_t kernel_physical_address, uint64_t kernel_size, uint64_t stack_physical_address, uint64_t stack_size) {
	DEBUG_PRINTF("Kernel loaded at phisical address: 0x%lx (size: %luMB). Stack at: 0x%lx (size: %luMB)\n", kernel_physical_address, kernel_size >> 20, stack_physical_address, stack_size >> 20);
}
