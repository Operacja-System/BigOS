#include <stddef.h>
#include <stdint.h>

struct llvm_libc_stdio_cookie {};

struct llvm_libc_stdio_cookie __llvm_libc_stdout_cookie;
struct llvm_libc_stdio_cookie __llvm_libc_stdin_cookie;
struct llvm_libc_stdio_cookie __llvm_libc_stderr_cookie;

static volatile unsigned char* uart = (volatile unsigned char*)0x10000000;

int __llvm_libc_stdio_read(void*, char*, size_t) {
	return 0;
}

int __llvm_libc_stdio_write(void*, char* buf, size_t sz) {
	for(size_t i = 0; i < sz; ++i) *uart = buf[i];
	return sz;
}
