/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/partition.h
 *  Description:	Interface for filesystems access.
 *
 *  Note:
 *  The name "partition" might be misleading since not all filesystems live
 *  on disk partitions. It is however safe to disregard this naming detail.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_PARTITION
#define BIGOS_BOOTLOADER_PARTITION

#include <efi.h>

#include "error.h"

typedef struct {
	UINTN flags;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_FILE_PROTOCOL* root;
	EFI_FILE_SYSTEM_INFO* file_system_info;
	EFI_DEVICE_PATH_PROTOCOL* device_path;
	EFI_GUID* guid;
} partition_t;

/**
 * @brief	Search for all avaible partitions and store their handles and data in g_partition_table
 */
[[nodiscard]] status_t partition_table_create(void);

/**
 * @brief	Print partition data
 *
 * @param	partition - Partition to print the data of
 */
void partition_print(partition_t* partition);

/**
 * @brief	Print all partition data
 */
void partition_table_print(void);

#endif // !BIGOS_BOOTLOADER_PARTITION
