#ifndef KERNEL_HAL_INCLUDE_HAL
#define KERNEL_HAL_INCLUDE_HAL

#include <hal/hal_interface.h>

typedef u64 (*syscall_handler_t)(u64 num, u64[8]);
typedef void (*irq_handler_t)();
typedef void (*timer_handler_t)();
typedef void (*fault_handler_t)();

hal_t hal_init(void* dtb);
void hal_set_handlers(syscall_handler_t syscall, irq_handler_t irq, timer_handler_t timer, fault_handler_t fault);
void hal_set_allocator(u64 (*allocate)(size_t size), void (*deallocate)(u64 address));

#endif // !KERNEL_HAL_INCLUDE_HAL
