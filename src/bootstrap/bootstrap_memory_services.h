#ifndef __BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__
#define __BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef struct {
	void* address;
	u64 size;
} physical_memory_region_t;

// TODO: This probably shouldn't be here
#define kiB (1ull << 10)
#define MiB (1ull << 20)
#define GiB (1ull << 30)

u64 align_up(u64 val, u64 align);
bool is_aligned(u64 val, u64 align);
void set_ram_params(void* ram_start, u64 ram_size);
void* get_ram_start();
u64 get_ram_size();

[[nodiscard]] error_t allocate_physical_memory_region(void* dt, u64 size, u64 align, physical_memory_region_t* pmrOUT);
[[nodiscard]] error_t load_elf_at_address(void* elf_img, void* target_addr, void** elf_entry_OUT);

#endif // !__BOOTSTRAP_BOOTSTRAP_MEMORY_SERVICES_H__
