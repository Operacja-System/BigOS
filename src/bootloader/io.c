/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/io.c
 *
 ******************************************************************************/

#include "io.h"

#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#include "error.h"
#include "log.h"

status_t read_file(EFI_FILE_PROTOCOL* file, UINTN offset, UINTN size, void* buffer) {
	START;
	EFI_STATUS status;
	unsigned char* buf = buffer;

	log(L"Set read position to %llu...", offset);
	status = file->SetPosition(file, offset);
	if (EFI_ERROR(status)) {
		err(L"Failed to set read offset. EFI_FILE_PROTOCOL.SetPosition() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Reading %llu bytes...", size);
	for (UINT64 read = 0; read < size;) {
		UINT64 remains = size - read;
		status = file->Read(file, &remains, (void*)(buf + read));
		if (EFI_ERROR(status))
			RETURN(BOOT_ERROR);
		read += remains;
	}

	RETURN(BOOT_SUCCESS);
}

status_t get_file_info(EFI_FILE_PROTOCOL* file, EFI_FILE_INFO** file_info) {
	START;
	EFI_STATUS status;
	UINTN size = 0;

	EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
	status = file->GetInfo(file, &file_info_guid, &size, *file_info);
	if (status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get file info size. EFI_FILE_PROTOCOL.GetInfo() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	*file_info = AllocateZeroPool(size);
	if (*file_info == nullptr) {
		err(L"Failed to allocate buffer for file info");
		RETURN(BOOT_ERROR);
	}

	status = file->GetInfo(file, &file_info_guid, &size, *file_info);
	if (EFI_ERROR(status)) {
		FreePool(*file_info);
		err(L"Failed to get file info. EFI_FILE_PROTOCOL.GetInfo() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

status_t get_directory_entry(EFI_FILE_PROTOCOL* file, UINTN* size, void** buffer) {
	START;
	EFI_STATUS status;
	*size = 0;

	status = file->Read(file, size, *buffer);
	if (EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get entry info size. EFI_FILE_PROTOCOL.Read() return code: %u", status);
		RETURN(BOOT_ERROR);
	} else if (status == EFI_SUCCESS) {
		*size = 0;
		RETURN(BOOT_SUCCESS);
	}

	*buffer = AllocatePool(*size);
	if (*buffer == nullptr) {
		err(L"Failed to allocate buffer for directory entry");
		RETURN(BOOT_ERROR);
	}

	status = file->Read(file, size, *buffer);
	if (EFI_ERROR(status)) {
		FreePool(*buffer);
		err(L"Failed to read entry. EFI_FILE_PROTOCOL.Read() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}
