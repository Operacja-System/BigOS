#ifndef RELOCATIONS_H_
#define RELOCATIONS_H_

#include <stdbool.h>
#include <stdint.h>

typedef void (*reloc_function_t)(void);

typedef struct {
	const reloc_function_t* preinit_array;
	uintptr_t preinit_count;
	reloc_function_t init;
	const reloc_function_t* init_array;
	uintptr_t init_count;
	reloc_function_t fini;
	const reloc_function_t* fini_array;
	uintptr_t fini_count;
} reloc_init_arrays_t;

typedef struct {
	void* load_addr;
	const void* rela;
	uintptr_t relasz;
	uintptr_t relaent;
	reloc_init_arrays_t init_arrays;
} reloc_dynamic_info_t;

bool reloc_load_dynamic_info(reloc_dynamic_info_t* info);
bool self_relocate(const reloc_dynamic_info_t* info);

#endif
