/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/pt_load.h
 *  Description:	ELF load segment handling
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF_PT_LOAD
#define BIGOS_BOOTLOADER_ELF_PT_LOAD

#include "elf_defines.h"
#include "error.h"

typedef struct elf_application_t elf_application_t;

/**
 * @brief	Load the PT_LOAD segment into executable memory
 *
 * @param	app - ELF application to load the PT_LOAD segment of
 */
status_t load_pt_load(elf_application_t* app, Elf64_Phdr* prog_header);

#endif // !BIGOS_BOOTLOADER_ELF_PT_LOAD
