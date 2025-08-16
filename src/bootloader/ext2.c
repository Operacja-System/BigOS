/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/ext2.c
 *
 ******************************************************************************/

#include "ext2.h"

#include <efi.h>
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>
#include <efilib.h>

#include "common.h"
#include "error.h"
#include "log.h"

#define EXT2_DRIVER_PATH L"EFI\\BOOT\\ext2.efi"

status_t ext2_driver_start(void) {
	START;
	status_t status;

	log(L"Opening file...");
	EFI_FILE_PROTOCOL* ext2_file;
	status = g_loader.root->Open(g_loader.root, &ext2_file, EXT2_DRIVER_PATH, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (EFI_ERROR(status)) {
		err(L"Failed to open file. EFI_FILE_PROTOCOL.Open() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Loading driver image...");
	EFI_DEVICE_PATH_PROTOCOL* ext2_driver_path = FileDevicePath(g_loader.image->DeviceHandle, EXT2_DRIVER_PATH);
	EFI_HANDLE ext2_driver_handle;
	status = g_system_table->BootServices->LoadImage(FALSE, g_image_handle, ext2_driver_path, nullptr, 0,
	                                                 &ext2_driver_handle);
	if (EFI_ERROR(status)) {
		err(L"Failed to load driver image. BootServices.LoadImage() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Starting driver...");
	status = g_system_table->BootServices->StartImage(ext2_driver_handle, nullptr, nullptr);
	if (EFI_ERROR(status)) {
		err(L"Failed to start ext2 driver. BootServices.StartImage() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Closing file...");
	ext2_file->Close(ext2_file);

	RETURN(BOOT_SUCCESS);
}
