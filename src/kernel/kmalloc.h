#ifndef _KERNEL_KMALLOC_H_
#define _KERNEL_KMALLOC_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

[[nodiscard]] error_t saint_kmalloc(size_t size, void** ptrOUT);
[[nodiscard]] error_t kmalloc(size_t size, void** ptrOUT);
void kfree(void* ptr);

#endif //!_KERNEL_KMALLOC_H_
