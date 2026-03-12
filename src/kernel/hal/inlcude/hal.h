#ifndef KERNEL_HAL_INCLUDE_HAL
#define KERNEL_HAL_INCLUDE_HAL

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "memory_management/include/common_types.h"
#include "memory_management/include/physical_memory/manager.h"

/// @ingroup hal
typedef enum : u16 {
	HALMF_READ = 1u << 0,
	HALMF_WRITE = 1u << 1,
	HALMF_EXECUTE = 1u << 2,
	HALMF_GLOBAL = 1u << 3,
	HALMF_PRIVILEGED = 1u << 4,
} hal_memory_flags_t;

/// @ingroup hal
typedef enum {
	HALPS_SMALL,
	HALPS_MEDIUM,
	HALPS_LARGE,
	HALPS_HUGE,
} hal_page_selection_t;

typedef u64 hal_arch_flags_t;

/**
 * @ingroup hal
 * @brief Generic hardware-specific address space handler.
 *
 * This type is fully opaque to the caller.
 * Do not read or modify its contents directly.
 */
typedef u8 hardware_address_space_t[64];

/**
 * @ingroup hal
 * @brief Hardware abstraction layer interface.
 * Contains function pointers and metadata for hardware specific operations the kernel needs to perform.
 */
typedef struct {
	physical_memory_region_t dtb;
	size_t address_space_size;
	size_t reserved_regions_count;
	size_t memory_regions_count;
	u32 smallest_page_size;
	/**
	 * Page size in the range of <1MiB, 1GiB), if something in this range is not available, will default to the biggest
	 * available page size, smaller then 1MiB.
	 * */
	u32 medium_page_size;
	/**
	 * Page size in the range of <1GiB, 512GiB), if something in this range is not available, will default to the
	 * biggest available page size.
	 * */
	u32 large_page_size;
	u32 biggest_page_size;
	hal_arch_flags_t flags;
} hal_t;

/**
 * @ingroup hal
 * */
[[gnu::nonnull(2)]]
error_t hal_init(physical_memory_region_t dtb, hal_t* halOUT);

/**
 * @ingroup hal
 * @brief Reads reserved memory regions from device tree.
 * Copies reserved memory regions entries from device tree blob into memory provided by the caller.
 *
 * @param memOUT Effective address of an array where regions will be copied too.
 * Must be at least `hal->reserved_regions_count * sizeof(memory_area_t)`.
 *
 * @retval ERR_NONE
 * */
[[gnu::nonnull(2)]]
error_t hal_get_reserved_regions(hal_t hal, memory_area_t* memOUT);

/**
 * @ingroup hal
 * @brief Reads memory regions from device tree.
 * Copies memory regions entries from device tree blob into memory provided by the caller.
 *
 * @param memOUT Effective address of an array where regions will be copied too.
 * Must be at least `hal->memory_regions_count * sizeof(memory_region_t)`.
 *
 * @retval ERR_NONE Success.
 * */
[[gnu::nonnull(2)]]
error_t hal_get_memory_regions(hal_t hal, memory_region_t* memOUT);

/**
 * @ingroup hal
 * @brief Maps a physical memory region into virtual memory.
 * Creates entries in the page table by mapping a contiguous range of virtual memory pages
 * to frames from a contiguous range of physical memory.
 *
 * @param as Address space handle in which the mappings will occur.
 * @param pmpm Physical memory region to map.
 * Must be aligned to the selected page size specified in `ps`.
 * Will be aligned up to the nearest multiple of the page size if necessary.
 * @param vmpm Virtual memory region to map.
 * Must be aligned to the selected page size specified in `ps`.
 * Will be aligned up to the nearest multiple of the page size if necessary.
 * @param ps Desired page size to use for the mapping.
 * @param flags Memory access and attribute flags.
 *
 * @retval ERR_NONE
 * @retval ERR_NOT_VALID @p pmem or @p vmem are not properly aligned.
 * @retval ERR_INVALID_MEMORY_REGION @p vmem is outside the address space range (0 - `hal->address_space_size`).
 * @retval ERR_PHYSICAL_MEMORY_FULL Failed to allocate memory for intermediate pages because physical memory is
 * full.
 * @retval ERR_ALLOCATION_FAILED Failed to allocate memory for another reason.
 *
 * @note Pages refer to chunks of virtual (or effective) memory, whereas frames refer to chunks of physical memory.
 * @todo The virtual memory management system (of which this function is a part)
 * should be able to manage effective memory regions even when an MMU is not available.
 * (important since Coreblocks doesn't implement it for now)
 * */
[[gnu::nonnull(2)]]
error_t hal_map_virtual_memory_region(hal_t hal, hardware_address_space_t* as, memory_area_t pmem, memory_area_t vmem,
                                      hal_page_selection_t ps, hal_memory_flags_t flags);

/**
 * @ingroup hal
 * @todo To be defined...
 * */
error_t hal_enable_virtual_memory(hal_t hal);

/// @ingroup hal
void hal_flush_tlb(hal_t hal);

/**
 * @ingroup hal
 * Flushes all translation look-ahead buffer entries for a given address space.
 * @retval ERR_NONE
 * @retval ERR_NOT_VALID Address space is not valid.
 * */
error_t hal_flush_tlb_address_space(hal_t hal, hardware_address_space_t*);

/**
 * @ingroup hal
 * @todo implement
 * */
error_t hal_init_times_interupts(hal_t hal);

/**
 * @ingroup hal
 * @todo implement
 * */
error_t hal_set_timer(hal_t hal, u64 timeout /*some lable?*/);

/**
 * @ingroup hal
 * @todo implement
 * */
error_t hal_init_irq(hal_t hal);

/**
 * @ingroup hal
 * Enables interrupt requests.
 * @retval ERR_NONE
 * @retval ERR_NOT_INITIALIZED Any interrupt request handles has not been set.
 * */
error_t hal_enable_irq(hal_t hal);

/// @ingroup hal
void hal_disable_irq(hal_t hal);

#endif // !KERNEL_HAL_INCLUDE_HAL
