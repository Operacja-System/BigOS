/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf.h
 *  Description:	ELF file handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF
#define BIGOS_BOOTLOADER_ELF

#include <efi.h>

#include "error.h"

typedef struct {
	unsigned char ident[16];
	UINT16 type;
	UINT16 machine;
	UINT32 version;
	UINT64 entry;
	UINT64 phoff;
	UINT64 shoff;
	UINT32 flags;
	UINT16 ehsize;
	UINT16 phentsize;
	UINT16 phnum;
	UINT16 shentsize;
	UINT16 shnum;
	UINT16 shstrndx;
} elf64_header_t;

typedef struct {
	UINT32 type;
	UINT32 flags;
	UINT64 offset;
	UINT64 vaddr;
	UINT64 paddr;
	UINT64 filesz;
	UINT64 memsz;
	UINT64 align;
} elf_program_header_t;

typedef struct {
	UINT32 name;
	UINT32 type;
	UINT64 flags;
	UINT64 addr;
	UINT64 offset;
	UINT64 size;
	UINT32 link;
	UINT32 info;
	UINT64 addralign;
	UINT64 entsize;
} elf_section_header_t;

typedef struct {
	UINT32 name;
	unsigned char info;
	unsigned char other;
	UINT16 shndx;
	UINT64 value;
	UINT64 size;
} elf_symbol_table_t;

typedef struct {
	UINT64 offset;
	UINT64 info;
	INT64 addend;
} elf_rela_t;

typedef struct {
	elf_section_header_t* symtab_hdr;
	elf_section_header_t* rela_dyn_hdr;
	elf_section_header_t* rela_plt_hdr;
	elf_section_header_t* strtab_hdr;
	char* strtab;
	elf_symbol_table_t* symtab;
} elf_relocations_t;

typedef struct {
	EFI_FILE_PROTOCOL* file;
	elf64_header_t header;
	elf_program_header_t* program_headers;
	elf_section_header_t* section_headers;
	CHAR8* section_headers_strings;
	elf_relocations_t relocations;

	UINT64 entry_address;

	UINT64 img_begin;
	UINT64 img_end;
	UINT64 base_vaddr;
	UINT64 top_vaddr;
	UINT64 page_count;
	UINT64 size;
	UINT64 physical_base;
} elf_application_t;

[[nodiscard]] status_t elf_load(elf_application_t* app);

#endif // !BIGOS_BOOTLOADER_ELF
