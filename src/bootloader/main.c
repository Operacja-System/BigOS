/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/main.c
 *  Description:	Entry point for GRUBAS bootloader.
 *
 *  Note:
 *  It's best to have UEFI documentation on hand when reading this code.
 *
 ******************************************************************************/

#include <efi.h>
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>
#include <efilib.h>

#include "common.h"
#include "config.h"
#include "elf/elf.h"
#include "error.h"
#include "exit.h"
#include "ext2.h"
#include "fdt.h"
#include "kernel.h"
#include "loader.h"
#include "log.h"
#include "partition.h"

EFI_HANDLE g_image_handle;
EFI_SYSTEM_TABLE* g_system_table;
elf_application_t g_kernel_app;

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
	status_t status;
	g_image_handle = image_handle;
	g_system_table = system_table;
	InitializeLib(image_handle, g_system_table);

	Print(L"GRUB for Academic Systems v0.1\n");
	Print(L"    ______ ____   __  __ ____   ___    _____\n");
	Print(L"   / ____// __ \\ / / / // __ ) /   |  / ___/\n");
	Print(L"  / / __ / /_/ // / / // __  |/ /| |  \\__ \\ \n");
	Print(L" / /_/ // _, _// /_/ // /_/ // ___ | ___/ / \n");
	Print(L" \\____//_/ |_| \\____//_____//_/  |_|/____/  \n");
	Print(L"\n");
	log(L"UEFI-boot running...");

	log(L"Initializing loader...");
	status = initialize_loader();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to initialize loader");
		exit();
	}

	log(L"Getting FDT...");
	status = get_FDT();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to get FDT");
		exit();
	}

	log(L"Loading meta config...");
	status = meta_config_load();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to load meta config");
		exit();
	}

	log(L"Starting ext2 driver...");
	status = ext2_driver_start();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to run ext2 driver");
		exit();
	}

	log(L"Creating partition table...");
	status = partition_table_create();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to create partition table");
		exit();
	}

	log(L"Listing all partitions...");
	partition_table_print();

	log(L"Loading config...");
	status = config_load();
	if (status != BOOT_SUCCESS) {
		err(L"Failed to load config");
		exit();
	}

	g_kernel_app.file = g_config.kernel;

	log(L"Loading kernel...");
	status = elf_load(&g_kernel_app);
	if (status != BOOT_SUCCESS) {
		err(L"Failed to load kernel");
		exit();
	}

	log(L"Starting kernel...");
	kernel_start();
	err(L"Kernel returned");
	exit();
}
