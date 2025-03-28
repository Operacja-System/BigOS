#include <stdbigos/string.h>
#include <drivers/uart.h>

#define ENTER 0xd
#define BACKSPACE 0x100
#define BUFFER_SIZE 4096

char console_buffer[BUFFER_SIZE];

int readline(void) {
  int console_read = 0;
  int c;
  while ((c = getc_uart()) !=  ENTER) {
    if (c == -1) continue;

    if (c == '\x7f' || c == 8) {
      if (console_read != 0) {
        puts_uart("\b \b");
        console_read--;
      }
    }
    else if(console_read != BUFFER_SIZE - 1) {
      if(console_read < 0 || console_read >= BUFFER_SIZE)
        puts_uart("\nAssertion failed in readline!!\n");

      console_buffer[console_read] = c;
      console_read++;
      putc_uart(c);
    }
  }
  console_buffer[console_read] = '\0';

  return console_read;
}

int process_command(char *comm) {
  if (!strcmp(comm, "help")) {
    puts_uart("\nIt's only this lousy command for now mate");
  }
  else if (!strcmp(comm, "ping")) {
    puts_uart("\npong");
  }
  return 0;
}
