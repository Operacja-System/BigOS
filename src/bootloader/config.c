/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/config.c
 *
 ******************************************************************************/

#include "config.h"

#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#include "common.h"
#include "error.h"
#include "exit.h"
#include "guid.h"
#include "io.h"
#include "log.h"
#include "partition.h"

#define META_CONFIG_PATH L"EFI\\BOOT\\conf.meta"

meta_config_t g_meta_config;
config_t g_config;

void meta_config_unload(void) {
	START;
	log(L"Deleting meta config data...");
	FreePool(g_meta_config.path);
	END;
}

status_t meta_config_load(void) {
	START;
	EFI_STATUS status;
	status_t boot_status;

	EFI_FILE_PROTOCOL* meta_config_file;

	log(L"Opening file...");
	status =
	    g_loader.root->Open(g_loader.root, &meta_config_file, META_CONFIG_PATH, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (EFI_ERROR(status)) {
		err(L"Failed to open file. EFI_FILE_PROTOCOL.Open() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Reading file info...");
	EFI_FILE_INFO* file_info = nullptr;
	boot_status = get_file_info(meta_config_file, &file_info);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read GUID");
		meta_config_file->Close(meta_config_file);
		RETURN(BOOT_ERROR);
	}

	log(L"Verifying file size...");
	if (file_info->FileSize < sizeof(EFI_GUID)) {
		FreePool(file_info);
		meta_config_file->Close(meta_config_file);
		err(L"Invalid pre-config file data");
		RETURN(BOOT_ERROR);
	}

	log(L"Reading file contents...");
	boot_status = read_file(meta_config_file, 0, sizeof(EFI_GUID), (void*)&g_meta_config);
	if (boot_status != BOOT_SUCCESS) {
		FreePool(file_info);
		meta_config_file->Close(meta_config_file);
		err(L"Failed to read GUID");
		RETURN(BOOT_ERROR);
	}

	UINTN path_size = file_info->FileSize - sizeof(EFI_GUID);
	FreePool(file_info);
	CHAR16* path = AllocatePool(path_size + 2);
	if (path == nullptr) {
		meta_config_file->Close(meta_config_file);
		err(L"Failed to allocate buffer for file data");
		RETURN(BOOT_ERROR);
	}

	boot_status = read_file(meta_config_file, sizeof(EFI_GUID), path_size, (void*)path);
	if (boot_status != BOOT_SUCCESS) {
		FreePool(path);
		meta_config_file->Close(meta_config_file);
		err(L"Failed to read path");
		RETURN(BOOT_ERROR);
	}
	path[path_size / 2] = L'\0';

	g_meta_config.path = path;
	g_meta_config.path_size = path_size + 2;
	exit_procedure_register(meta_config_unload);

	log(L"Closing file...");
	meta_config_file->Close(meta_config_file);

	RETURN(BOOT_SUCCESS);
}

void config_unload(void) {
	START;
	log(L"Deleting config data...");
	g_config.kernel->Close(g_config.kernel);
	g_config.directory->Close(g_config.directory);
	END;
}

status_t config_load(void) {
	START;
	EFI_STATUS status;
	status_t boot_status;

	log(L"Searching for config partition...");
	partition_t* partition = nullptr;
	for (UINTN i = 0; i < g_partition_table_count; ++i) {
		partition_t* current = &g_partition_table[i];
		if (current->guid == nullptr)
			continue;
		// HACK: CompareGuid always returns 0 here for some reason
		if (guid_compare(current->guid, &g_meta_config.partition_guid)) {
			partition = current;
			break;
		}
	}
	if (partition == nullptr) {
		err(L"Failed to find config partition");
		RETURN(BOOT_ERROR);
	}

	log(L"Opening config at %s...", g_meta_config.path);
	status = partition->root->Open(partition->root, &g_config.directory, g_meta_config.path, EFI_FILE_MODE_READ,
	                               EFI_FILE_READ_ONLY);
	if (EFI_ERROR(status)) {
		err(L"Failed to open file. EFI_FILE_PROTOCOL.Open() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	log(L"Reading file info...");
	EFI_FILE_INFO* config_dir_info = nullptr;
	boot_status = get_file_info(g_config.directory, &config_dir_info);
	if (boot_status != BOOT_SUCCESS) {
		g_config.directory->Close(g_config.directory);
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	if (!(config_dir_info->Attribute & EFI_FILE_DIRECTORY)) {
		g_config.directory->Close(g_config.directory);
		FreePool(config_dir_info);
		err(L"Config path does not lead to directory");
		RETURN(BOOT_ERROR);
	}
	FreePool(config_dir_info);

	log(L"Reading config dir contents...");
	START;
	while (1) {
		EFI_FILE_INFO* entry_info;
		UINTN entry_size;
		boot_status = get_directory_entry(g_config.directory, &entry_size, (void**)&entry_info);
		if (boot_status != BOOT_SUCCESS) {
			g_config.directory->Close(g_config.directory);
			err(L"Failed to read directory contents");
			END;
			RETURN(BOOT_ERROR);
		}

		if (entry_size == 0)
			break;

		if (StrLen(entry_info->FileName) == 0 || StrCmp(entry_info->FileName, L".") == 0 ||
		    StrCmp(entry_info->FileName, L"..") == 0) {
			FreePool(entry_info);
			continue;
		}

		EFI_FILE_PROTOCOL* entry;
		status = g_config.directory->Open(g_config.directory, &entry, entry_info->FileName, EFI_FILE_MODE_READ,
		                                  EFI_FILE_READ_ONLY);
		if (EFI_ERROR(status)) {
			FreePool(entry_info);
			err(L"Failed to open entry %s. EFI_FILE_PROTOCOL.Open() return code: %u", entry_info->FileName, status);
			continue;
		}

		if (entry_info->Attribute & EFI_FILE_DIRECTORY) {
			log(L"Found directory %s", entry_info->FileName);
			entry->Close(entry);
		} else {
			if (StrCmp(entry_info->FileName, L"kernel") == 0) {
				log(L"Found kernel binary", entry_info->FileName);
				g_config.kernel = entry;
			} else {
				log(L"Found file %s", entry_info->FileName);
				entry->Close(entry);
			}
		}

		FreePool(entry_info);
	}
	END;

	exit_procedure_register(config_unload);

	RETURN(BOOT_SUCCESS);
}
