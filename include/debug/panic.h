#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

void panic(const char* msg);
void error(const char* msg);

#define KERNEL_PANIC_H_STRINGIFY(X)  #X
#define KERNEL_PANIC_H_STRINGIFYD(X) KERNEL_PANIC_H_STRINGIFY(X)

#define KERNEL_PANIC_H_STRING __FILE__ "@" KERNEL_PANIC_H_STRINGIFYD(__LINE__) ": "

#ifndef PANIC
	#define PANIC(cstr)                   \
		do {                              \
			error(KERNEL_PANIC_H_STRING); \
			panic(cstr);                  \
		} while (0)
#endif

#ifndef PANIC_E
	#define PANIC_E(err)                  \
		do {                              \
			error(KERNEL_PANIC_H_STRING); \
			panic(get_error_msg(err));    \
		} while (0)
#endif

#ifndef ERROR
	#define ERROR(cstr)                   \
		do {                              \
			error(KERNEL_PANIC_H_STRING); \
			error(cstr);                  \
		} while (0)
#endif
#ifndef ERROR_E
	#define ERROR_E(err)                  \
		do {                              \
			error(KERNEL_PANIC_H_STRING); \
			error(get_error_msg(err));    \
		} while (0)
#endif

#endif
