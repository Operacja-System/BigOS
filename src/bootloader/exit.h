/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/exit.h
 *  Description:	Helper module for ensuring proper boot exit.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_EXIT
#define BIGOS_BOOTLOADER_EXIT

#include <efi.h>
#include <efidef.h>

/**
 * @brief	Exit EFI loader with error
 */
[[noreturn]]
void exit(void);

/**
 * @brief	Prepare to pass control to the kernel
 *
 * @note	EFI boot services become unavaible after calling this function
 */
void exit_boot(void);

/**
 * @brief	Add function to run when bootloader exits
 *
 * @param	function - Pointer to a function meant to be called when bootloader exits
 *
 * @note	Exit procedures are called when exit() or exit_boot() are called
 */
void exit_procedure_register(void (*function)(void));

#endif // !BIGOS_BOOTLOADER_EXIT
