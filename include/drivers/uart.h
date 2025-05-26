#ifndef UART_H
#define UART_H
void init_uart(void);
void putc_uart(int c);
void puts_uart(char* s);
int getc_uart(void);
#endif
