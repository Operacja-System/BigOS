/* utils.h

Code snippet provides a way to safely print out
debbuging messages in RiscV OpenSBI environment. 

*/

#include "stddef.h"
#include "stdint.h"

void uart_putchar(char c);
void uart_print(const char *s);
void uart_print_num(long num);
void uart_print_hex(uintptr_t num);