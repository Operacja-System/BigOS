#ifndef KERNEL_ADDRESS_SPACE_ADDRESS_SPACE
#define KERNEL_ADDRESS_SPACE_ADDRESS_SPACE

#include "stdbigos/error.h"
#include "stdbigos/types.h"

typedef enum : u16 {
	AS_FLAGS_READ = (1ull << 0),
	AS_FLAGS_WRITE = (1ull << 1),
	AS_FLAGS_EXECUTE = (1ull << 2),
	AS_FLAGS_MAPPED = (1ull << 3),
	AS_FLAGS_GLOBAL = (1ull << 4),
	AS_FLAGS_USER = (1ull << 5),
} address_space_region_flags_t;

typedef struct {
	address_space_region_flags_t flags;
	void* addr;
	size_t size;
} address_space_region_t;

typedef struct {
	address_space_region_t* regions;
	size_t regions_count;
	bool does_manage_own_memory;
	bool active;
} address_space_t;

error_t address_space_init();

error_t address_space_init_from_buffer(address_space_region_t* regions, size_t count);

error_t address_space_delegate_memory_management(void** addrOUT);

#endif // !KERNEL_ADDRESS_SPACE_ADDRESS_SPACE
