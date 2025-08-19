/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/elf.c
 *
 ******************************************************************************/

#include "elf.h"

#include <efi.h>
#include <efilib.h>
#include <stddef.h>

#include "common.h"
#include "elf_defines.h"
#include "error.h"
#include "io.h"
#include "log.h"
#include "pt_dynamic.h"
#include "pt_load.h"

#define PAGE_SIZE 4096

static status_t verify_elf_header(Elf64_Ehdr* header) {
	START;
	if (header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 ||
	    header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3) {
		err(L"No ELF signature: %u %c%c%c", (UINT8)header->e_ident[EI_MAG0], header->e_ident[EI_MAG1],
		    header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
		RETURN(BOOT_ERROR);
	}

	if (header->e_type != ET_DYN) { // PIE executables have a e_type of ET_DYN, not ET_EXEC
		err(L"Unsupported ELF e_type: %u", header->e_type);
		RETURN(BOOT_ERROR);
	}

	if (header->e_ident[EI_CLASS] != ELFCLASS64) {
		err(L"Unsupported ELF class: %u", (UINT8)header->e_ident[EI_CLASS]);
		RETURN(BOOT_ERROR);
	}

	if (header->e_phnum == 0) {
		err(L"No program headers");
		RETURN(BOOT_ERROR);
	}
	if (header->e_phentsize != sizeof(Elf64_Phdr)) {
		err(L"Unexpected program header size: %llu", header->e_phentsize);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t elf_program_headers_read(elf_application_t* app) {
	START;
	status_t boot_status;

	app->program_headers = AllocatePool((UINTN)app->header.e_phnum * app->header.e_phentsize);
	if (app->program_headers == nullptr) {
		err(L"Failed to allocate memory for program headers");
		RETURN(BOOT_ERROR);
	}

	boot_status = read_file(app->file, app->header.e_phoff, (UINTN)app->header.e_phnum * app->header.e_phentsize,
	                        app->program_headers);
	if (boot_status != BOOT_SUCCESS) {
		FreePool(app->program_headers);
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static void elf_program_headers_delete(elf_application_t* app) {
	FreePool(app->program_headers);
}

static status_t initialize_image_info(elf_application_t* app) {
	START;
	app->img_begin = UINT64_MAX;
	app->img_end = 0;
	for (UINTN i = 0; i < app->header.e_phnum; i++) {
		Elf64_Phdr* header = &app->program_headers[i];
		if (header->p_type != PT_LOAD)
			continue;
		if (header->p_vaddr < app->img_begin) {
			app->img_begin = header->p_vaddr;
		}
		if (header->p_vaddr + header->p_memsz > app->img_end) {
			app->img_end = header->p_vaddr + header->p_memsz;
		}
	}

	if (app->img_begin == UINT64_MAX) {
		err(L"No Load segments encountered");
		RETURN(BOOT_ERROR);
	}

	UINT64 page_mask = PAGE_SIZE - 1;
	app->base_vaddr = app->img_begin & ~page_mask;
	app->top_vaddr = (app->img_end + page_mask) & ~page_mask;
	app->size = app->top_vaddr - app->base_vaddr;
	app->page_count = app->size / PAGE_SIZE;

	RETURN(BOOT_SUCCESS);
}

static status_t load_segments(elf_application_t* app) {
	START;

	for (UINTN i = 0; i < app->header.e_phnum; ++i) {
		Elf64_Phdr* prog_header = &app->program_headers[i];

		switch (prog_header->p_type) {
		case PT_LOAD:
			log(L"Loading PT_LOAD segment...");
			if (load_pt_load(app, prog_header) != BOOT_SUCCESS)
				RETURN(BOOT_ERROR);
			break;
		case PT_DYNAMIC:
			log(L"Loading PT_DYNAMIC segment...");
			if (load_pt_dynamic(app, prog_header) != BOOT_SUCCESS)
				RETURN(BOOT_ERROR);
			break;
		default: warn(L"Unhandled program header type: %u", prog_header->p_type);
		}
	}

	log(L"Applying PT_DYNAMIC segment...");
	process_dt_dynamic(app);

	RETURN(BOOT_SUCCESS);
}

status_t elf_load(elf_application_t* app) {
	START;
	EFI_STATUS status;
	status_t boot_status;

	log(L"Reading ELF header...");
	boot_status = read_file(app->file, 0, sizeof(Elf64_Ehdr), &app->header);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read ELF header");
		RETURN(BOOT_ERROR);
	}

	log(L"Verifying ELF header...");
	boot_status = verify_elf_header(&app->header);
	if (boot_status != BOOT_SUCCESS) {
		err(L"ELF header verification failed");
		RETURN(BOOT_ERROR);
	}

	log(L"Reading program headers...");
	boot_status = elf_program_headers_read(app);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read program headers");
		RETURN(BOOT_ERROR);
	}

	log(L"Determining image size...");
	boot_status = initialize_image_info(app);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed memory needed");
		RETURN(BOOT_ERROR);
	}

	log(L"Allocating pages...");
	status = g_system_table->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, app->page_count,
	                                                     &app->physical_base);
	if (EFI_ERROR(status)) {
		err(L"Failed to allocate pages. BootServices.AllocatePages() return code: %u", status);
		RETURN(BOOT_ERROR);
	}
	SetMem((void*)app->physical_base, app->size, 0);

	log(L"Loading ELF segments...");
	boot_status = load_segments(app);
	if (boot_status != BOOT_SUCCESS) {
		g_system_table->BootServices->FreePages(app->physical_base, app->page_count);
		err(L"Failed to load ELF segments");
		RETURN(BOOT_ERROR);
	}

	elf_program_headers_delete(app);

	log(L"Determining entry point address...");
	if (app->header.e_entry < app->img_begin || app->header.e_entry >= app->img_end) {
		err(L"Invalid entry point address");
		RETURN(BOOT_ERROR);
	}
	app->entry_address = REBASE(app, app->header.e_entry);

	asm volatile("fence.i" ::: "memory");

	RETURN(BOOT_SUCCESS);
}
