#ifndef __BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__
#define __BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef struct {
	void* address;
	u64 size;
} phisical_memory_region_t;

u64 align_up(u64 val, u64 align);
void set_ram_params(void* ram_start, u64 ram_size);
void* get_ram_start();
u64 get_ram_size();

error_t allocate_phisical_memory_region(void* dt, phisical_memory_region_t busy_memory_regions[],
										u64 busy_memory_regions_amount, u64 allocation_size, u64 aligment,
										phisical_memory_region_t* pmrOUT);

error_t load_elf_at_address(void* elf_img, void* target_addr, void** elf_entry_OUT);

#endif // !__BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__
