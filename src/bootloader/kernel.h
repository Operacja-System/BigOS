/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/kernel.h
 *  Description:	Kernel execution handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_KERNEL
#define BIGOS_BOOTLOADER_KERNEL

/**
 * @brief	Start the kernel and exit UEFI environment
 *
 * @note	This function should not return if OS is loaded correctly
 */
[[noreturn]]
void kernel_start(void);

#endif // !BIGOS_BOOTLOADER_KERNEL
