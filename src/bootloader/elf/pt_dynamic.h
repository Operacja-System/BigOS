/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/pt_dynamic.h
 *  Description:	ELF dynamic segment handling
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF_PT_DYNAMIC
#define BIGOS_BOOTLOADER_ELF_PT_DYNAMIC

#include "elf_defines.h"
#include "error.h"

typedef struct elf_application_t elf_application_t;

typedef struct {
	const char* strtab;
	UINTN strsz;

	Elf64_Sym* symtab;
	UINTN syment;

	Elf64_Rela* rela;
	UINTN rela_sz;
	UINTN rela_ent;

	Elf64_Rel* rel;
	UINTN rel_sz;
	UINTN rel_ent;

	void (*init)(void);
	void (**init_array)(void);
	UINTN init_array_sz;

	void (*fini)(void);
	void (**fini_array)(void);
	UINTN fini_array_sz;

	void* dt_debug;
} elf_pt_dynamic_t;

/**
 * @brief	Loads info about PT_DYNAMIC segment (if there is such)
 *
 * @param	app - ELF application that contains the segment
 * @param	prog_header - the PT_DYNAMIC segment header
 */
status_t load_pt_dynamic(elf_application_t* app, Elf64_Phdr* prog_header);

/**
 * @brief	Process the dynamic segment (ie. relocations, static initialization)
 *
 * @param	app - ELF application to process the dynamic segment of
 */
status_t process_dt_dynamic(elf_application_t* app);

#endif // !BIGOS_BOOTLOADER_ELF_PT_DYNAMIC
