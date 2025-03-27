#include <stdbigos/sbi.h>

sbiret sbi_ecall(SbiExtensionId ext, reg_t fid, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5) {
	register reg_t a0 __asm__("a0") = arg0;
	register reg_t a1 __asm__("a1") = arg1;
	register reg_t a2 __asm__("a2") = arg2;
	register reg_t a3 __asm__("a3") = arg3;
	register reg_t a4 __asm__("a4") = arg4;
	register reg_t a5 __asm__("a5") = arg5;
	register reg_t a6 __asm__("a6") = fid;
	register reg_t a7 __asm__("a7") = ext;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");
	return (sbiret){.error = a0, .uvalue = a1};
}

sbiret sbi_debug_console_write(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi) {
	return sbi_ecall(SbiExtDebugConsole, 0, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
}
sbiret sbi_debug_console_read(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi) {
	return sbi_ecall(SbiExtDebugConsole, 1, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
}
sbiret sbi_debug_console_write_byte(u8 byte) {
	return sbi_ecall(SbiExtDebugConsole, 2, byte, 0, 0, 0, 0, 0);
}
