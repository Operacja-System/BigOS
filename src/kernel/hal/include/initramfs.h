#ifndef HAL_INITRAMFS
#define HAL_INITRAMFS

#include "stdbigos/buffer.h"
#include "stdbigos/error.h"

error_t hal_initramfs_read(const char* filename, buffer_t* buffOUT);

#endif // !HAL_INITRAMFS
