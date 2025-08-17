/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf/pt_dynamic.c
 *
 ******************************************************************************/

#include "pt_dynamic.h"

#include "elf/elf.h"
#include "error.h"
#include "log.h"

#define R_RISCV_64        2
#define R_RISCV_RELATIVE  3

status_t load_pt_dynamic(elf_application_t* app, Elf64_Phdr* prog_header) {
	START;

	elf_pt_dynamic_t* info = &app->pt_dynamic_info;
	Elf64_Dyn* dyn = (Elf64_Dyn*)REBASE(app, prog_header->p_vaddr);
	UINTN dyn_count = prog_header->p_filesz / sizeof(Elf64_Dyn);

	for (UINTN i = 0; i < dyn_count && dyn[i].d_tag != DT_NULL; ++i) {
		Elf64_Sxword tag = dyn[i].d_tag;
		Elf64_Xword val = dyn[i].d_un.d_val;
		switch (tag) {
		case DT_STRTAB: info->strtab = (const char*)REBASE(app, val); break;
		case DT_STRSZ:  info->strsz = (UINTN)val; break;
		case DT_SYMTAB: info->symtab = (Elf64_Sym*)REBASE(app, val); break;
		case DT_SYMENT: info->syment = (UINTN)val; break;

		case DT_RELA:    info->rela = (Elf64_Rela*)REBASE(app, val); break;
		case DT_RELASZ:  info->rela_sz = (UINTN)val; break;
		case DT_RELAENT: info->rela_ent = (UINTN)val; break;

		case DT_REL:    info->rel = (Elf64_Rel*)REBASE(app, val); break;
		case DT_RELSZ:  info->rel_sz = (UINTN)val; break;
		case DT_RELENT: info->rel_ent = (UINTN)val; break;

		case DT_INIT:         info->init = (void (*)(void))REBASE(app, val); break;
		case DT_INIT_ARRAY:   info->init_array = (void (**)(void))REBASE(app, val); break;
		case DT_INIT_ARRAYSZ: info->init_array_sz = (UINTN)val; break;

		case DT_FINI:         err(L"DT_FINI entries aren't supported."); RETURN(BOOT_ERROR);
		case DT_FINI_ARRAY:   err(L"DT_FINI_ARRAY entries aren't supported."); RETURN(BOOT_ERROR);
		case DT_FINI_ARRAYSZ: err(L"DT_FINI_ARRAYSZ entries aren't supported."); RETURN(BOOT_ERROR);

		case DT_PLTREL:   err(L"DT_PLTREL entries aren't supported."); RETURN(BOOT_ERROR);
		case DT_JMPREL:   err(L"DT_JMPREL entries aren't supported."); RETURN(BOOT_ERROR);
		case DT_PLTRELSZ: err(L"DT_PLTRELSZ entries aren't supported."); RETURN(BOOT_ERROR);

		case DT_NEEDED: err(L"DT_NEEDED entries aren't supported."); RETURN(BOOT_ERROR);

		case DT_DEBUG: info->dt_debug = (void*)REBASE(app, val); break;

		case DT_GNU_HASH: warn(L"Ignored DT_GNU_HASH entry."); break;
		case DT_HASH:     warn(L"Ignored DT_HASH entry."); break;

		default: warn(L"Ignored unknown dynamic entry."); break;
		}
	}

	RETURN(BOOT_SUCCESS);
}

static status_t apply_rela(elf_application_t* app) {
	START;
	const size_t count = app->pt_dynamic_info.rela_sz / sizeof(Elf64_Rela);
	Elf64_Rela* rela = app->pt_dynamic_info.rela;

	for (size_t i = 0; i < count; ++i) {
		UINT32 type = ELF64_R_TYPE(rela[i].r_info);
		UINT32 sym = ELF64_R_SYM(rela[i].r_info);
		UINT64 offset = rela[i].r_offset;
		INT64 addend = rela[i].r_addend;

		UINT64* patch_addr = (UINT64*)REBASE(app, offset);

		switch (type) {
		case R_RISCV_RELATIVE:
			log(L"Applied relocation type: R_RISCV_RELATIVE.");
			*patch_addr = app->physical_base + addend;
			break;
		case R_RISCV_64:
			log(L"Applied relocation type: R_RISCV_64.");
			UINT64 symval = app->pt_dynamic_info.symtab[sym].st_value;
			*patch_addr = app->physical_base + symval + addend;
			break;
		default: err(L"Unsupported RELA relocation type: %u", type); RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

static status_t apply_rel(elf_application_t* app) {
	START;
	const size_t count = app->pt_dynamic_info.rel_sz / sizeof(Elf64_Rela);
	Elf64_Rel* rel = app->pt_dynamic_info.rel;

	for (size_t i = 0; i < count; ++i) {
		UINT32 type = ELF64_R_TYPE(rel[i].r_info);
		UINT32 sym = ELF64_R_SYM(rel[i].r_info);
		UINT64 offset = rel[i].r_offset;

		UINT64* patch_addr = (UINT64*)REBASE(app, offset);

		switch (type) {
		case R_RISCV_RELATIVE:
			log(L"Applied relocation type: R_RISCV_RELATIVE.");
			*patch_addr = app->physical_base + *patch_addr;
			break;
		case R_RISCV_64:
			log(L"Applied relocation type: R_RISCV_64.");
			UINT64 symval = app->pt_dynamic_info.symtab[sym].st_value;
			*patch_addr = app->physical_base + symval + *patch_addr;
			break;
		default: err(L"Unsupported REL relocation type: %u", type); RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

static void run_init(elf_application_t* app) {
	elf_pt_dynamic_t* info = &app->pt_dynamic_info;

	if (info->init) {
		log(L"Running init()...");
		info->init();
	}
	if (info->init_array && info->init_array_sz) {
		log(L"Running init_array()...");
		UINTN count = info->init_array_sz / sizeof(void (*)(void));
		for (size_t i = 0; i < count; ++i) {
			if (info->init_array[i])
				info->init_array[i]();
		}
	}
}

status_t process_dt_dynamic(elf_application_t* app) {
	START;

	elf_pt_dynamic_t* info = &app->pt_dynamic_info;

	if (info->rela && info->rela_sz) {
		log(L"Applying RELA relocations...");
		if (apply_rela(app) != BOOT_SUCCESS)
			RETURN(BOOT_ERROR);
	}

	if (info->rel && info->rel_sz) {
		log(L"Applying REL relocations...");
		if (apply_rel(app) != BOOT_SUCCESS)
			RETURN(BOOT_ERROR);
	}

	run_init(app);

	RETURN(BOOT_SUCCESS);
}
