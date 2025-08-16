/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/pt_load.c
 *
 ******************************************************************************/

#include "pt_load.h"

#include "elf/elf.h"
#include "elf_defines.h"
#include "error.h"
#include "io.h"
#include "log.h"

status_t load_pt_load(elf_application_t* app, Elf64_Phdr* prog_header) {
	START;
	status_t boot_status;
	UINT64 filesz = prog_header->p_filesz;
	UINT64 offset = prog_header->p_offset;
	UINT64 vaddr = prog_header->p_vaddr;
	UINT64 physical_dest = REBASE(app, vaddr);

	if (vaddr < app->base_vaddr || (vaddr + filesz) > app->top_vaddr) {
		err(L"Invalid segment memory range");
		RETURN(BOOT_ERROR);
	}

	boot_status = read_file(app->file, offset, filesz, (void*)physical_dest);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}
	RETURN(BOOT_SUCCESS);
}
