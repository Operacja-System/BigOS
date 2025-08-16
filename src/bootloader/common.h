/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/common.h
 *  Description:	Commonly used global values.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_COMMON
#define BIGOS_BOOTLOADER_COMMON

#include <efi.h>

#include "config.h"
#include "elf/elf.h"
#include "loader.h"
#include "partition.h"

extern EFI_HANDLE g_image_handle;
extern EFI_SYSTEM_TABLE* g_system_table;
extern loader_t g_loader;
extern partition_t* g_partition_table;
extern UINTN g_partition_table_count;
extern meta_config_t g_meta_config;
extern config_t g_config;
extern elf_application_t g_kernel_app;
extern void* g_fdt;

#endif // !BIGOS_BOOTLOADER_COMMON
