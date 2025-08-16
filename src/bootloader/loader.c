/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/loader.c
 *
 ******************************************************************************/

#include "loader.h"

#include <efi.h>

#include "common.h"
#include "error.h"
#include "log.h"

loader_t g_loader;

status_t initialize_loader(void) {
	START;
	EFI_STATUS status;

	log(L"Getting LoadedImageProtocol...");
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	status =
	    g_system_table->BootServices->HandleProtocol(g_image_handle, &loaded_image_protocol, (void**)&g_loader.image);
	if (EFI_ERROR(status)) {
		err(L"Failed to get image. BootServices.HandleProtocol() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Getting FileSystemProtocol...");
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->HandleProtocol(g_loader.image->DeviceHandle, &file_system_protocol,
	                                                      (void**)&g_loader.file_system);
	if (EFI_ERROR(status)) {
		err(L"Failed to get file system. BootServices.HandleProtocol() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Opening boot volume...");
	status = g_loader.file_system->OpenVolume(g_loader.file_system, &g_loader.root);
	if (EFI_ERROR(status)) {
		err(L"Failed to open volume. EFI_FILE_SYSTEM_PROTOCOL.OpenVolume() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}
