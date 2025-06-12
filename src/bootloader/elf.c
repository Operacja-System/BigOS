/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf.c
 *
 ******************************************************************************/

#include "elf.h"

#include <efi.h>
#include <efilib.h>

#include "common.h"
#include "error.h"
#include "io.h"
#include "log.h"

#define PAGE_SIZE 4096

#define SHT_SYMTAB 2
#define SHT_DYNSYM 11
#define SHT_RELA   4

#define R_RISCV_64        2
#define R_RISCV_RELATIVE  3
#define R_RISCV_COPY      4
#define R_RISCV_JUMP_SLOT 5

static status_t verify_elf_header(elf64_header_t* header) {
	START;
	if (header->ident[0] != 0x7f || header->ident[1] != 'E' || header->ident[2] != 'L' || header->ident[3] != 'F') {
		err(L"No ELF signature: %u %c%c%c", (UINT8)header->ident[0], header->ident[1], header->ident[2],
		    header->ident[3]);
		RETURN(BOOT_ERROR);
	}

	if (header->type != 2) {
		err(L"Unsupported ELF type: %u", header->type);
		RETURN(BOOT_ERROR);
	}

	if (header->ident[4] != 2) {
		err(L"Unsupported ELF class: %u", (UINT8)header->ident[4]);
		RETURN(BOOT_ERROR);
	}

	if (header->phnum == 0) {
		err(L"No program headers");
		RETURN(BOOT_ERROR);
	}
	if (header->phentsize != sizeof(elf_program_header_t)) {
		err(L"Unexpected program header size: %llu", header->phentsize);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t read_elf_program_headers(elf_application_t* app) {
	START;
	status_t boot_status;

	app->program_headers = AllocatePool(app->header.phnum * app->header.phentsize);
	if (app->program_headers == nullptr) {
		err(L"Failed to allocate memory for program headers");
		RETURN(BOOT_ERROR);
	}

	boot_status =
	    read_file(app->file, app->header.phoff, app->header.phnum * app->header.phentsize, app->program_headers);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t read_elf_section_headers(elf_application_t* app) {
	START;
	status_t boot_status;

	app->section_headers = AllocatePool(app->header.shnum * app->header.shentsize);
	if (app->section_headers == nullptr) {
		err(L"Failed to allocate memory for section headers");
		RETURN(BOOT_ERROR);
	}

	boot_status =
	    read_file(app->file, app->header.shoff, app->header.shnum * app->header.shentsize, app->section_headers);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	elf_section_header_t* string_table_header = &app->section_headers[app->header.shstrndx];
	app->section_headers_strings = AllocatePool(string_table_header->size);
	if (app->section_headers_strings == nullptr) {
		err(L"Failed to allocate memory for section string table");
		RETURN(BOOT_ERROR);
	}

	boot_status =
	    read_file(app->file, string_table_header->offset, string_table_header->size, app->section_headers_strings);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t initialize_image_info(elf_application_t* app) {
	START;
	app->img_begin = UINT64_MAX;
	app->img_end = 0;
	for (UINTN i = 0; i < app->header.phnum; i++) {
		elf_program_header_t* header = &app->program_headers[i];
		if (header->type != 1)
			continue;
		if (header->vaddr < app->img_begin) {
			app->img_begin = header->vaddr;
		}
		if (header->vaddr + header->memsz > app->img_end) {
			app->img_end = header->vaddr + header->memsz;
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
	status_t boot_status;

	for (UINTN i = 0; i < app->header.phnum; ++i) {
		elf_program_header_t* prog_header = &app->program_headers[i];
		if (prog_header->type != 1 /* LOAD */)
			continue;

		UINT64 vaddr = prog_header->vaddr;
		UINT64 filesz = prog_header->filesz;
		UINT64 offset = prog_header->offset;
		UINT64 segment_offset = vaddr - app->base_vaddr;
		UINT64 physical_dest = app->physical_base + segment_offset;

		if (vaddr < app->base_vaddr || (vaddr + filesz) > app->top_vaddr) {
			err(L"Invalid segment memory range");
			RETURN(BOOT_ERROR);
		}

		boot_status = read_file(app->file, offset, filesz, (void*)physical_dest);
		if (boot_status != BOOT_SUCCESS) {
			err(L"Failed to read file");
			RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

static status_t identify_relocations(elf_application_t* app) {
	START;
	status_t boot_status;

	log(L"Identifying relocation sections...");
	for (UINTN i = 0; i < app->header.shnum; ++i) {
		elf_section_header_t* section_h = &app->section_headers[i];
		const CHAR8* name = app->section_headers_strings + section_h->name;

		if (section_h->type == SHT_SYMTAB || section_h->type == SHT_DYNSYM) {
			app->relocations.symtab_hdr = section_h;
			app->relocations.strtab_hdr = &app->section_headers[section_h->link];
		} else if (section_h->type == SHT_RELA) {
			if (strcmpa(name, ".rela.dyn") == 0) {
				app->relocations.rela_dyn_hdr = section_h;
			} else if (strcmpa(name, ".rela.plt") == 0) {
				app->relocations.rela_plt_hdr = section_h;
			}
		}
	}

	if (app->relocations.symtab_hdr == nullptr) {
		err(L"No symbol table found");
	}

	log(L"Creating string table...");
	if (app->relocations.strtab_hdr != nullptr) {
		app->relocations.strtab = AllocatePool(app->relocations.strtab_hdr->size);
		if (app->relocations.strtab == nullptr) {
			err(L"Failed to allocate memory for string table");
			RETURN(BOOT_ERROR);
		}

		boot_status = read_file(app->file, app->relocations.strtab_hdr->offset, app->relocations.strtab_hdr->size,
		                        app->relocations.strtab);
		if (boot_status != BOOT_SUCCESS) {
			err(L"Failed to read file");
			RETURN(BOOT_ERROR);
		}
	}

	log(L"Creating symbol table...");
	if (app->relocations.symtab_hdr != nullptr) {
		app->relocations.symtab = AllocatePool(app->relocations.symtab_hdr->size);
		if (app->relocations.symtab == nullptr) {
			err(L"Failed to allocate memory for symbol table");
			RETURN(BOOT_ERROR);
		}

		boot_status = read_file(app->file, app->relocations.symtab_hdr->offset, app->relocations.symtab_hdr->size,
		                        app->relocations.symtab);
		if (boot_status != BOOT_SUCCESS) {
			err(L"Failed to read file");
			RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

#define ELF64_R_SYM(i)     ((i) >> 32)
#define ELF64_R_TYPE(i)    ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))

static status_t apply_rela_section(elf_application_t* app, elf_section_header_t* rela_hdr) {
	START;
	status_t boot_status;

	UINTN rela_entries_count = (rela_hdr->size / sizeof(elf_rela_t));

	elf_rela_t* rela_buf = AllocatePool(rela_hdr->size);
	if (rela_buf == nullptr) {
		err(L"Failed to allocate for relocation buffer");
		RETURN(BOOT_ERROR);
	}

	boot_status = read_file(app->file, rela_hdr->offset, rela_hdr->size, rela_buf);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read relocation entries");
		RETURN(BOOT_ERROR);
	}

	for (UINTN i = 0; i < rela_entries_count; ++i) {
		elf_rela_t* rel = &rela_buf[i];
		UINT32 type = ELF64_R_TYPE(rel->info);
		UINT32 sym = ELF64_R_SYM(rel->info);
		UINT64 offset = rel->offset;
		INT64 addend = rel->addend;

		UINT64 patch_addr = app->physical_base + (offset - app->base_vaddr);

		switch (type) {
		case R_RISCV_RELATIVE: {
			UINT64 new_val = app->physical_base + addend;
			*((UINT64*)patch_addr) = new_val;
			break;
		}

		case R_RISCV_64: {
			UINT64 symval = app->relocations.symtab[sym].value;
			UINT64 new_val = app->physical_base + symval + addend;
			*((UINT64*)patch_addr) = new_val;
			break;
		}

		default: {
			err(L"Relocation type %u not supported", type);
			RETURN(BOOT_ERROR);
		}
		}
	}
	FreePool(rela_buf);

	RETURN(BOOT_SUCCESS);
}

static status_t apply_relocations(elf_application_t* app) {
	START;
	status_t boot_status;

	if (app->relocations.rela_dyn_hdr != nullptr) {
		log(L"Applying .rela.dyn relocation");
		boot_status = apply_rela_section(app, app->relocations.rela_dyn_hdr);
		if (boot_status != BOOT_SUCCESS) {
			err(L"Failed to relocate .rela.dyn");
			RETURN(BOOT_ERROR);
		}
	}

	if (app->relocations.rela_plt_hdr != nullptr) {
		log(L"Applying .rela.plt relocation");
		boot_status = apply_rela_section(app, app->relocations.rela_plt_hdr);
		if (boot_status != BOOT_SUCCESS) {
			err(L"Failed to relocate .rela.plt");
			RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

status_t elf_load(elf_application_t* app) {
	START;
	EFI_STATUS status;
	status_t boot_status;

	log(L"Reading ELF header...");
	boot_status = read_file(app->file, 0, sizeof(elf64_header_t), &app->header);
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
	boot_status = read_elf_program_headers(app);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read program headers");
		RETURN(BOOT_ERROR);
	}

	log(L"Reading section headers...");
	boot_status = read_elf_section_headers(app);
	if (boot_status != BOOT_SUCCESS) {
		err(L"Failed to read section headers");
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

	log(L"Identifying relocations...");
	boot_status = identify_relocations(app);
	if (boot_status != BOOT_SUCCESS) {
		g_system_table->BootServices->FreePages(app->physical_base, app->page_count);
		err(L"Failed to identify relocations");
		RETURN(BOOT_ERROR);
	}

	log(L"Applying relocations...");
	boot_status = apply_relocations(app);
	if (boot_status != BOOT_SUCCESS) {
		g_system_table->BootServices->FreePages(app->physical_base, app->page_count);
		err(L"Failed to apply relocations");
		RETURN(BOOT_ERROR);
	}

	log(L"Determining entry point address...");
	if (app->header.entry < app->img_begin || app->header.entry >= app->img_end) {
		err(L"Invalid entry point address");
		RETURN(BOOT_ERROR);
	}
	app->entry_address = app->physical_base + (app->header.entry - app->base_vaddr);

	RETURN(BOOT_SUCCESS);
}
