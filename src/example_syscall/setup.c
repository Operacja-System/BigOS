/* setup.c
Sets up environment for smooth testing experience...

*/

#include <stdint.h>
#include "sbi.h"

extern void trap_entry(void);
extern int main(void);
extern uint8_t _stack_top[];
extern uint8_t _kernel_stack_top[];

void _start() {
    // Set stack pointer
    __asm__ volatile ("mv sp, %0" : : "r"(_stack_top));

    // Set trap vector
    uintptr_t trap_entry_addr = (uintptr_t)trap_entry;
    __asm__ volatile ("csrw stvec, %0" : : "r"(trap_entry_addr));

    // Initialize sscratch with kernel stack top
    __asm__ volatile ("csrw sscratch, %0" : : "r"(_kernel_stack_top));

    // Enable interrupts
    uintptr_t sstatus_val;
    __asm__ volatile ("csrr %0, sstatus" : "=r"(sstatus_val));
    sstatus_val |= (1 << 1);  // SIE bit
    __asm__ volatile ("csrw sstatus, %0" : : "r"(sstatus_val));

    main();
    sbi_shutdown();
}
