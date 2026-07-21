/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/elf.h
 *  Description:	ELF file handling module and standard ELF64 defines.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF_ELF
#define BIGOS_BOOTLOADER_ELF_ELF

#include <efi.h>

#include "elf/pt_dynamic.h"
#include "elf_defines.h"
#include "error.h"

#define REBASE(app, vaddr) ((app)->physical_base + ((vaddr) - (app)->base_vaddr))

typedef struct elf_application_t {
	EFI_FILE_PROTOCOL* file;
	Elf64_Ehdr header;
	Elf64_Phdr* program_headers;
	elf_pt_dynamic_t pt_dynamic_info;

	UINT64 entry_address;

	UINT64 img_begin;
	UINT64 img_end;
	UINT64 base_vaddr;
	UINT64 top_vaddr;
	UINT64 page_count;
	UINT64 size;
	UINT64 physical_base;
} elf_application_t;

/**
 * @brief	Load the ELF file and setup its execution
 *
 * @param	app - ELF application to load
 */
[[nodiscard]] status_t elf_load(elf_application_t* app);

#endif // !BIGOS_BOOTLOADER_ELF_ELF
