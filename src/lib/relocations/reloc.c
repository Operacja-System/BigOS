#include <elf.h>
#include <relocations/reloc.h>
#include <stdbigos/types.h>
#include <stdint.h>

#define R_RISCV_NONE     0
#define R_RISCV_RELATIVE 3

// NOLINTNEXTLINE
extern const reg_t _GLOBAL_OFFSET_TABLE_[] __attribute__((visibility("hidden")));

/* Get link time address of _DYNAMIC e.g first entry in GOT */
static inline Elf64_Addr elf_get_dynamic_linktime() {
	return _GLOBAL_OFFSET_TABLE_[0];
}

static inline Elf64_Dyn* elf_get_dynamic_runtime() {
	Elf64_Dyn* dynamic;
	__asm__("lla %0, _DYNAMIC" : "=r"(dynamic));
	return dynamic;
}

bool reloc_load_dynamic_info(reloc_dynamic_info_t* info) {
	if (!info) {
		return false;
	}

	*info = (reloc_dynamic_info_t){0};

	Elf64_Dyn* dynamic = elf_get_dynamic_runtime();
	info->load_addr = (void*)((uintptr_t)dynamic - elf_get_dynamic_linktime());

	u8* ld_addr = (u8*)info->load_addr;

	for (; dynamic->d_tag != DT_NULL; dynamic++) {
		switch (dynamic->d_tag) {
		case DT_PREINIT_ARRAY:
			info->init_arrays.preinit_array = (const reloc_function_t*)(ld_addr + dynamic->d_un.d_ptr);
			break;
		case DT_PREINIT_ARRAYSZ:
			info->init_arrays.preinit_count = dynamic->d_un.d_val / sizeof(reloc_function_t);
			break;
		case DT_INIT: info->init_arrays.init = (reloc_function_t)(ld_addr + dynamic->d_un.d_ptr); break;
		case DT_INIT_ARRAY:
			info->init_arrays.init_array = (const reloc_function_t*)(ld_addr + dynamic->d_un.d_ptr);
			break;
		case DT_INIT_ARRAYSZ: info->init_arrays.init_count = dynamic->d_un.d_val / sizeof(reloc_function_t); break;
		case DT_FINI:         info->init_arrays.fini = (reloc_function_t)(ld_addr + dynamic->d_un.d_ptr); break;
		case DT_FINI_ARRAY:
			info->init_arrays.fini_array = (const reloc_function_t*)(ld_addr + dynamic->d_un.d_ptr);
			break;
		case DT_FINI_ARRAYSZ: info->init_arrays.fini_count = dynamic->d_un.d_val / sizeof(reloc_function_t); break;
		case DT_RELA:         info->rela = ld_addr + dynamic->d_un.d_ptr; break;
		case DT_RELASZ:       info->relasz = dynamic->d_un.d_val; break;
		case DT_RELAENT:      info->relaent = dynamic->d_un.d_val; break;
		default:              break;
		}
	}

	return true;
}

bool self_relocate(const reloc_dynamic_info_t* info) {
	if (!info) {
		return false;
	}

	if (!info->rela)
		return info->relaent == 0;

	if (info->relaent == 0)
		return info->rela == nullptr;

	for (uintptr_t off = 0; off < info->relasz; off += info->relaent) {
		const Elf64_Rela* cur_rel = (const Elf64_Rela*)((u8*)info->rela + off);
		switch (ELF64_R_TYPE(cur_rel->r_info)) {
		case R_RISCV_NONE: break;
		case R_RISCV_RELATIVE:
			void** addr = (void**)((u8*)info->load_addr + cur_rel->r_offset);
			*addr = (u8*)info->load_addr + cur_rel->r_addend;
			break;
		default:
			// unknown relocation type
			return false;
		}
	}

	return true;
}
