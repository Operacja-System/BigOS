/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/fdt.h
 *  Description:	FDT handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_FDT
#define BIGOS_BOOTLOADER_FDT

#include <efi.h>

#include "error.h"

/**
 * @brief	Read FDT from EFI system table and store it in g_fdt
 */
[[nodiscard]] status_t get_FDT(void);

#endif // !BIGOS_BOOTLOADER_FDT
