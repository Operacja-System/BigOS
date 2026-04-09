#ifndef LIB_HAL_HAL_INTERNAL
#define LIB_HAL_HAL_INTERNAL

#include "stdbigos/error.h"

bool ihal_is_init();
error_t ihal_get_dtb(void** dtbOUT);

#endif // !LIB_HAL_HAL_INTERNAL
