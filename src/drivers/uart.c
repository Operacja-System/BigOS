#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>

// http://byterunner.com/16550.html
#define RHR 0 // Receive Holding Register
#define THR 0 // Transmit Holding Register
#define IER 1 // Interrupt Enable Register
#define FCR 2 // FIFO Control Register
#define ISR 2 // Interrupt Status Register
#define LCR 3 // Line Control Register
#define MCR 4 // Modem Control Register
#define LSR 5 // Line Status Register
#define MSR 6 // Modem Status Register

#define LSR_THR_STATUS (1 << 5)
#define THR_FULL 0
#define THR_EMPTY 1

#define LSR_RHR_STATUS (1 << 0)
#define RHR_EMPTY 0
#define RHR_HAVE_DATA 1

// TODO we should probably declare such things in the linker script.
// also we should parse DST and not hardcode anything.
#define UART 0x10000000

// look at the table at the end of the document linked abode
#define BAUD_RATE_DIVISOR_LSB 3
#define BAUD_RATE_DIVISOR_MSB 0

#define Reg(reg) ((volatile unsigned char *)(UART + (reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))
#define ReadReg(reg) (*(Reg(reg)))

void init_uart(void) {
  // disable interrupts
  WriteReg(IER, 0x00);
  // go into set baud rate mode and set the freq
  WriteReg(LCR, 1 << 7);
  WriteReg(0, BAUD_RATE_DIVISOR_LSB);
  WriteReg(1, BAUD_RATE_DIVISOR_MSB);
  // 8 bits, 1 stop bit, no parity for now (also leave baud rate mode)
  WriteReg(LCR, 0b11);
  // go into polled mode
  // TODO change it to interrupt mode once we have processes
  WriteReg(FCR, 0x01);
  // i guess we're already in, but let's set this as stated in docs
  WriteReg(IER, 0x00);
}

void putc_uart(int c) {
  while((ReadReg(LSR) & LSR_THR_STATUS) == THR_FULL);
  WriteReg(THR, c);
}

int getc_uart(void){
  if((ReadReg(LSR) & LSR_RHR_STATUS) == RHR_HAVE_DATA) {
    return ReadReg(RHR);
  }
  else {
    return -1;
  }
}

void puts_uart(char *s) {
  while (*s) putc_uart(*s++);
}
