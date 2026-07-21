/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/fdt.c
 *
 ******************************************************************************/

#include "dt_bootutils.h"

// HACK
#include <../src/libboot/dt/dt_common.h>
#include <../src/libboot/dt/dt_defines.h>
#include <efi.h>
#include <efierr.h>
#include <efilib.h>
#include <libboot/dt/dt.h>
#include <libcore/bitutils.h>
#include <libcore/string.h>

#include "common.h"
#include "error.h"
#include "guid.h"
#include "log.h"

#define EFI_FDT_GUID                                                                  \
	{                                                                                 \
	    0xb1b621d5, 0xf19c, 0x41a5, {0x83, 0x0b, 0xd9, 0x15, 0x2c, 0x69, 0xaa, 0xe0} \
}

#define FDT_EXTRA_SPACE 4096

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
// FIXME: Idk g_bs wasn't defined, it's broken as of now
status_t expand_fdt_for_writing(void) {
	/*
	if (!g_fdt)
	    return BOOT_ERROR;

	u32 old_total_size = read_be32((u8*)g_fdt + FDT_OFF_TOTAL_SIZE);
	u32 new_total_size = old_total_size + FDT_EXTRA_SPACE;

	void* new_fdt = NULL;


	EFI_STATUS status = g_bs->AllocatePool(EfiLoaderData, new_total_size, &new_fdt);
	if (EFI_ERROR(status)) {
	    return BOOT_ERROR;
	}

	memcpy(new_fdt, g_fdt, old_total_size);
	write_be32((u8*)new_fdt + FDT_OFF_TOTAL_SIZE, new_total_size);

	g_fdt = new_fdt;
	*/
	return BOOT_SUCCESS;
}

error_t dt_add_subnode(fdt_t* fdt_obj, dt_node_t parent_node, const char* new_node_name) {
	if (fdt_obj == nullptr || new_node_name == nullptr)
		return ERR_BAD_ARG;

	u8* base = (u8*)fdt_obj->fdt_buffer.data;
	if (!base)
		return ERR_NOT_VALID;

	u32 insert_offset = parent_node;
	error_t err = dt_skip_node_name(fdt_obj, insert_offset, &insert_offset);
	if (err != ERR_NONE)
		return err;

	err = dt_skip_node_properties(fdt_obj, insert_offset, &insert_offset);
	if (err != ERR_NONE)
		return err;

	size_t name_len = strlen(new_node_name) + 1;
	size_t padded_name_len = align_u32(name_len, 4);

	// TOKEN_BEGIN + NAZWA + TOKEN_END
	u32 new_node_bytes = 4 + padded_name_len + 4;

	u32 bytes_to_move = fdt_obj->total_size - insert_offset;
	memmove(base + insert_offset + new_node_bytes, base + insert_offset, bytes_to_move);

	u32 curr_off = insert_offset;
	write_be32(base + curr_off, FDT_BEGIN_NODE);
	curr_off += 4;
	memset(base + curr_off, 0, padded_name_len);
	strcpy((char*)(base + curr_off), new_node_name);
	curr_off += padded_name_len;

	write_be32(base + curr_off, FDT_END_NODE);
	fdt_obj->struct_size += new_node_bytes;
	fdt_obj->total_size += new_node_bytes;

	if (fdt_obj->strings_off > fdt_obj->struct_off) {
		fdt_obj->strings_off += new_node_bytes;
		write_be32(base + FDT_OFF_OFF_DT_STRINGS, fdt_obj->strings_off);
	}

	write_be32(base + FDT_OFF_SIZE_DT_STRUCT, fdt_obj->struct_size);
	write_be32(base + FDT_OFF_TOTAL_SIZE, fdt_obj->total_size);

	return ERR_NONE;
}
