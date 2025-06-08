/* test.c
This code snippet simulates user interaction with syscall wrapper.

In short, it first changes Risc-V firmware running on S-mode to U-mode,
then it returns to test() function which calls our syscall.

*/

#include "syscall.h"
#include "sbi.h"

__attribute__((noreturn)) void test() {
    uart_print("Testing...\n");
    riscv_syscall(1, (long)"Hello World\n", 0, 0, 0, 0, 0);

    uart_print("Test was successfull!\n");
    
    while (1);

    /* Code never reaches there */
}

int main(void) {
    uintptr_t sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1 << 8); 
    __asm__ volatile("csrw sstatus, %0" : : "r"(sstatus));
    
    __asm__ volatile("csrw sepc, %0" : : "r"(test));
    
    __asm__ volatile("sret");
    
    return 0;
}
