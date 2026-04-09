#ifndef HAL_ADDRESS_SPACE
#define HAL_ADDRESS_SPACE

#include "stdbigos/error.h"
#include "stdbigos/memory_types.h"
#include "stdbigos/types.h"

typedef struct {
	alignas(64) u8 data[32];
} hal_address_space_t;

typedef enum : u16 {
	HAL_ASR_FLAGS_READ = (1ull << 0),
	HAL_ASR_FLAGS_WRITE = (1ull << 1),
	HAL_ASR_FLAGS_EXECUTE = (1ull << 2),
} hal_address_region_flag_t;

typedef enum : u16 {
	HAL_AS_GLOBAL = (1ull << 0),
	HAL_AS_USER = (1ull << 1),
} hal_address_space_flag_t;

error_t hal_enable_virtual_address_spaces(hal_address_space_t* initial_as);

error_t hal_address_space_init(hal_address_space_t* as, hal_address_space_flag_t flags);

/**
 * @brief Gets an array of available frame sizes
 *
 * Returns an array of available frame sizes in ascending order
 *
 * @param countOUT Pointer to a variable where the size of the array will be written to
 * @returns Pointer to the first element of the array
 * */
[[nodiscard]] [[gnu::nonnull]]
const u32* hal_get_available_frame_sizes(u32* countOUT);

error_t hal_address_space_set_active(hal_address_space_t* as);

error_t hal_address_space_map(hal_address_space_t as, uintptr_t vaddr, physical_memory_region_t pmem,
                              hal_address_region_flag_t flags);

error_t hal_address_space_unmap(hal_address_space_t as, uintptr_t vaddr, physical_memory_region_t* pmemOUT);

#endif // !HAL_ADDRESS_SPACE
