/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/config.h
 *  Description:	Bootloader configuration data handling.
 *
 *  Note:
 *  GRUBAS config may be located on a different partition than the bootloader
 *  itself. For this reason, the localization of the config is stored in
 *  conf.meta (meta config) file on boot partition.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_CONFIG
#define BIGOS_BOOTLOADER_CONFIG

#include <efi.h>
#include <efidef.h>

#include "error.h"

typedef struct {
	EFI_GUID partition_guid;
	CHAR16* path;
	UINTN path_size;
} meta_config_t;

typedef struct {
	EFI_FILE_PROTOCOL* directory;
	EFI_FILE_PROTOCOL* kernel;
	// EFI_FILE_PROTOCOL** required;
	// UINTN required_count;
} config_t;

/**
 * @brief	Reads conf.meta file and stores data in g_meta_config
 *
 * @note	conf.meta has to be located in EFI/BOOT/ on boot partition
 * @note	The conf.meta is a raw binary that is copied byte by byte:
 *			First 16 bytes are copied as EFI_GUID struct.
 *			Next bytes are copied to a CHAR16* buffer.
 */
[[nodiscard]] status_t meta_config_load(void);

[[nodiscard]] status_t config_load(void);

#endif // !BIGOS_BOOTLOADER_CONFIG
