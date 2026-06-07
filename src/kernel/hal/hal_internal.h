#ifndef HAL_HAL_INTERNAL
#define HAL_HAL_INTERNAL

#include <libcore/error.h>

bool ihal_is_init();
error_t ihal_get_dtb(void** dtbOUT);

#endif // !HAL_HAL_INTERNAL
