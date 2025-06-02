#ifndef __BOOTSTRAP_BOOTSTRAP__PANIC_H__
#define __BOOTSTRAP_BOOTSTRAP__PANIC_H__

#include <stdbigos/types.h>

#define PANIC(msg) bootstrap_panic(__FILE__, __LINE__, msg)
[[noreturn]] void bootstrap_panic(const char* file_name, u64 line, const char* msg);

#endif // !__BOOTSTRAP_BOOTSTRAP_PANIC_H__
