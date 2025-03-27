#include <debug/debug_stdio.h>
#include <stdint.h>

#define RAM_START 0x80000000
#define KENREL_PHISCAL_ADDRESS RAM_START
#define KERNEL_MAX_SIZE 0x400000 //NOTE: 4MB
#define KERNEL_STACK_SIZE 0x200000 //NOTE: 2MB
#define KERNEL_STACK_PHISICAL_ADDRESS (KERNEL_PHISICAL_ADDRESS + KERNEL_MAX_SIZE + KERNEL_STACK_SIZE)

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

void kinit(uint64_t origin_physical_address) {
	DEBUG_PRINTF("Kernel loaded at phisical address: 0x%lx\n", origin_physical_address);
}
