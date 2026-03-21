#ifndef HAL_HAL
#define HAL_HAL

#include <stdbigos/address.h>
#include <stdbigos/error.h>

/// @addtogroup hal
/// @{

/**
 * @param dtb Physical address of device tree blob
 * @param physical_to_effective A pointer to a function that transalates physical addresses into effective ones
 *
 * @retval ERR_NONE Success
 * @retval ERR_BAD_ARG @p dtb is null
 * @retval ERR_NOT_VALID No valid device tree found at address @p dtb
 * */
[[gnu::nonnull(2)]]
error_t hal_init(__phys void* dtb, void* (*physical_to_effective)(__phys void*));

/// @}

#endif // !HAL_HAL
