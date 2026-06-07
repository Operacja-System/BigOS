/**
 * @file memory_regions.h
 * @brief Memory region enumeration API for HAL
 * @ingroup hal_riscv_mem
 */

#ifndef HAL_MEMORY_REGIONS
#define HAL_MEMORY_REGIONS

#include <libcore/error.h>
#include <libcore/memory_types.h>

/// @addtogroup hal_riscv_mem
/// @{

/**
 * This type is opaque to the caller.
 * Fields of this struct should not be modified directly.
 * */
typedef struct {
	alignas(16) u8 data[32];
} hal_reserved_memory_iterator_t;

/**
 * Initialize an iterator for enumerating system-reserved memory regions.
 *
 * Reserved regions come from two sources in the device tree:
 * - The /reserved-memory node and its children
 * - The memory reservation table (memreserve entries)
 *
 * @param iterOUT Pointer to uninitialized iterator to fill.
 *
 * @retval ERR_NONE Iterator initialized successfully
 * @retval ERR_BAD_ARG Invalid argument (null parameter or device tree validation error)
 * @retval ERR_NOT_INITIALIZED HAL is not initialized
 * @retval Other error codes Device tree parsing failures
 *
 * @note Upon error, @p iterOUT should remain unchanged.
 */
error_t hal_get_reserved_regions_iterator(hal_reserved_memory_iterator_t* iterOUT);

/**
 * Retrieve the next reserved memory region.
 *
 * Call this repeatedly after hal_get_reserved_regions_iterator() until it returns ERR_NOT_FOUND.
 *
 * @param iter Pointer to active iterator (initialized by hal_get_reserved_regions_iterator())
 * @param areaOUT Pointer to output structure. On success, filled with region details.
 *
 * @retval ERR_NONE Region retrieved successfully
 * @retval ERR_BAD_ARG Invalid argument (null parameter or device tree validation error)
 * @retval ERR_NOT_FOUND No more regions (end of iteration)
 * @retval Other error codes Device tree errors
 *
 * @note Upon error, @p areaOUT should remain unchanged.
 */
error_t hal_get_next_reserved_region(hal_reserved_memory_iterator_t* iter, memory_area_t* areaOUT);

/**
 * This type is opaque to the caller.
 * Fields of this struct should not be modified directly.
 * */
typedef struct {
	alignas(16) u8 data[32];
} hal_memory_iterator_t;

/**
 * Initialize an iterator for enumerating physical memory regions.
 *
 * Enumerates all /memory nodes from the device tree, including all register entries
 * within each memory node.
 *
 * @param iterOUT Pointer to uninitialized iterator to fill.
 *
 * @retval ERR_NONE Iterator initialized successfully
 * @retval ERR_BAD_ARG Invalid argument (null parameter or device tree validation error)
 * @retval ERR_NOT_INITIALIZED HAL is not initialized
 * @retval Other error codes Device tree parsing failures
 *
 * @note Upon error, @p iterOUT should remain unchanged.
 */
error_t hal_get_memory_regions_iterator(hal_memory_iterator_t* iterOUT);

/**
 * Retrieve the next available memory region.
 *
 * Call this repeatedly after hal_get_memory_regions_iterator() until it returns ERR_NOT_FOUND.
 * Each call retrieves one register entry from the device tree memory nodes.
 *
 * @param iter Pointer to active iterator (initialized by hal_get_memory_regions_iterator())
 * @param areaOUT Pointer to output structure. On success, filled with region details.
 *
 * @retval ERR_NONE Region retrieved successfully
 * @retval ERR_BAD_ARG Invalid argument (null parameter or device tree validation error)
 * @retval ERR_NOT_FOUND No more regions (end of iteration)
 * @retval Other error codes Device tree errors
 *
 * @note Upon error, @p areaOUT should remain unchanged.
 */
error_t hal_get_next_memory_region(hal_memory_iterator_t* iter, physical_memory_region_t* areaOUT);

/// @}

#endif // !HAL_MEMORY_REGIONS
