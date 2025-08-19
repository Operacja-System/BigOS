/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/fdt.c
 *
 ******************************************************************************/

#include "fdt.h"

#include <efierr.h>
#include <efilib.h>

#include "common.h"
#include "error.h"
#include "guid.h"
#include "log.h"

#define EFI_FDT_GUID                                                                  \
	{                                                                                 \
	    0xb1b621d5, 0xf19c, 0x41a5, {0x83, 0x0b, 0xd9, 0x15, 0x2c, 0x69, 0xaa, 0xe0} \
}

void* g_fdt;

// FDT is created by u-boot and then passed into UEFI system table
status_t get_FDT(void) {
	START;
	EFI_GUID fdt_guid = EFI_FDT_GUID;
	EFI_CONFIGURATION_TABLE* entry;

	for (UINTN index = 0; index < g_system_table->NumberOfTableEntries; ++index) {
		entry = &g_system_table->ConfigurationTable[index];
		if (guid_compare(&entry->VendorGuid, &fdt_guid)) {
			g_fdt = entry->VendorTable;
			log(L"FDT address: %lX", g_fdt);
			RETURN(BOOT_SUCCESS);
		}
	}

	RETURN(BOOT_ERROR);
}
