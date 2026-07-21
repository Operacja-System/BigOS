/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/partition.c
 *
 ******************************************************************************/

#include "partition.h"

#include <efi.h>
#include <efilib.h>

#include "common.h"
#include "error.h"
#include "exit.h"
#include "log.h"

partition_t* g_partition_table;
UINTN g_partition_table_count;

void partition_table_free(void) {
	START;
	log(L"Deleting partition table...");
	for (UINTN i = 0; i < g_partition_table_count; ++i) {
		if (g_partition_table[i].flags == 0)
			continue;
		FreePool(g_partition_table[i].file_system_info);
		g_partition_table[i].root->Close(g_partition_table[i].root);
	}
	FreePool(g_partition_table);
	END;
}

static void partition_create(partition_t* partition, EFI_HANDLE handle) {
	EFI_STATUS status;

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->HandleProtocol(handle, &file_system_protocol, (void**)&file_system);
	if (EFI_ERROR(status))
		return;

	EFI_FILE_PROTOCOL* root;
	status = file_system->OpenVolume(file_system, &root);
	if (EFI_ERROR(status))
		return;

	EFI_FILE_SYSTEM_INFO* file_system_info = nullptr;
	UINTN file_system_info_size = 0;
	EFI_GUID file_system_info_guid = EFI_FILE_SYSTEM_INFO_ID;
	status = root->GetInfo(root, &file_system_info_guid, &file_system_info_size, file_system_info);
	if (status != EFI_BUFFER_TOO_SMALL) {
		root->Close(root);
		return;
	}

	file_system_info = AllocatePool(file_system_info_size);
	if (file_system_info == nullptr) {
		root->Close(root);
		return;
	}

	status = root->GetInfo(root, &file_system_info_guid, &file_system_info_size, file_system_info);
	if (EFI_ERROR(status)) {
		FreePool(file_system_info);
		root->Close(root);
		return;
	}

	EFI_DEVICE_PATH_PROTOCOL* device_path;
	EFI_GUID device_path_guid = EFI_DEVICE_PATH_PROTOCOL_GUID;
	status = g_system_table->BootServices->HandleProtocol(handle, &device_path_guid, (void**)&device_path);
	if (EFI_ERROR(status)) {
		FreePool(file_system_info);
		root->Close(root);
		return;
	}

	EFI_GUID* part_guid = nullptr;
	EFI_DEVICE_PATH_PROTOCOL* node = device_path;
	while (!IsDevicePathEnd(node)) {
		if (DevicePathType(node) == MEDIA_DEVICE_PATH && DevicePathSubType(node) == MEDIA_HARDDRIVE_DP) {
			HARDDRIVE_DEVICE_PATH* harddrive_path = (HARDDRIVE_DEVICE_PATH*)node;
			if (harddrive_path->SignatureType == SIGNATURE_TYPE_GUID) {
				part_guid = (EFI_GUID*)harddrive_path->Signature;
				break;
			}
		}
		node = NextDevicePathNode(node);
	}

	partition->flags = 1;
	partition->file_system = file_system;
	partition->root = root;
	partition->file_system_info = file_system_info;
	partition->device_path = device_path;
	partition->guid = part_guid;
}

status_t partition_table_create(void) {
	START;
	EFI_STATUS status;

	log(L"Locating file system handles...");
	EFI_HANDLE* file_systems_table;
	UINTN file_systems_count;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->LocateHandleBuffer(ByProtocol, &file_system_protocol, nullptr,
	                                                          &file_systems_count, &file_systems_table);
	if (EFI_ERROR(status)) {
		err(L"Failed to locate file system handles. BootServices.LocateHandleBuffer() return code: %u", status);
		RETURN(BOOT_ERROR);
	}

	g_partition_table_count = file_systems_count;
	g_partition_table = AllocateZeroPool(sizeof(partition_t) * file_systems_count);
	if (EFI_ERROR(status)) {
		err(L"Failed to allocate memory for partition data");
		FreePool(file_systems_table);
		RETURN(BOOT_ERROR);
	}

	log(L"Creating partition table...");
	for (UINTN i = 0; i < g_partition_table_count; ++i) {
		partition_create(&g_partition_table[i], file_systems_table[i]);
	}

	exit_procedure_register(partition_table_free);

	FreePool(file_systems_table);
	RETURN(BOOT_SUCCESS);
}

void partition_print(partition_t* partition) {
	START;
	if (partition->flags == 0) {
		err(L"Partition unavaible");
		END;
		return;
	}

	log(L"Partition avaible:");
	log(L" - Volume Label: '%s'", partition->file_system_info->VolumeLabel);
	log(L" - Volume Size: %llu", partition->file_system_info->VolumeSize);
	if (partition->guid != nullptr) {
		log(L" - GPT UUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", partition->guid->Data1,
		    partition->guid->Data2, partition->guid->Data3, partition->guid->Data4[0], partition->guid->Data4[1],
		    partition->guid->Data4[2], partition->guid->Data4[3], partition->guid->Data4[4], partition->guid->Data4[5],
		    partition->guid->Data4[6], partition->guid->Data4[7]);
	} else {
		warn(L" - GPT UUID: (missing)");
	}
	END;
}

void partition_table_print(void) {
	START;
	for (UINTN i = 0; i < g_partition_table_count; ++i) {
		log(L"Partition (%u):", i);
		partition_print(&g_partition_table[i]);
	}
	END;
}
