#include "utils.h"

void uart_putchar(char c) {
    volatile char *uart = (volatile char *)0x10000000L;
    *uart = c;
}

void uart_print(const char *s) {
    if (s == NULL) {
        const char* null_msg = "(NULL)";

        while(*null_msg) {
            uart_putchar(*null_msg++);
        }

        return;
    }

    while (*s) {
        uart_putchar(*s++);
    }
}

void uart_print_num(long num) {
    char buf[20];
    int i = 0;

    if (num == 0) {
        uart_putchar('0');
        return;
    }

    if (num < 0) {
        uart_putchar('-');
        num = -num;
    }

    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i--) {
       uart_putchar(buf[i]); 
    }
}

void uart_print_hex(uintptr_t num) {
    const char *digits = "0123456789ABCDEF";
    char buf[20];
    int i = 0;
    
    if(num == 0) {
        uart_putchar('0');
        return;
    }
    
    while(num) {
        buf[i++] = digits[num & 0xF];
        num >>= 4;
    }
    
    uart_print("0x");
    while(i--) {
        uart_putchar(buf[i]);
    }
}
