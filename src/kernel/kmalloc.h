#ifndef _KERNEL_KMALLOC_H_
#define _KERNEL_KMALLOC_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

[[nodiscard]] error_t kmalloc_set_working_set(void* wset_addr, u64 wset_size);
[[nodiscard]] error_t kmalloc(size_t size, void** ptrOUT);
void kfree(void* ptr);

#endif //!_KERNEL_KMALLOC_H_
