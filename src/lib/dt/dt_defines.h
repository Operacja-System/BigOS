/**
 * @ingroup dt_internal
 * @file dt_defines.h
 * @brief Magic number, offsets for the device tree header and minimum compatible version.
 */

#ifndef SRC_LIB_DT_DEFINES
#define SRC_LIB_DT_DEFINES

/// @addtogroup dt_internal
/// @ingroup dt
/// @{

/**
 * @name Defines
 *  @{
 */
#define FDT_MAGIC                 0xd00dfeed /**< FDT magic number. */
#define FDT_OFF_MAGIC             0x00       /**< Offset to magic number. */
#define FDT_OFF_TOTAL_SIZE        0x04       /**< Offset to total size. */
#define FDT_OFF_OFF_DT_STRUCT     0x08       /**< Offset to structure block offset. */
#define FDT_OFF_OFF_DT_STRINGS    0x0C       /**< Offset to strings block offset. */
#define FDT_OFF_OFF_MEM_RSVMAP    0x10       /**< Offset to memory reservation map offset. */
#define FDT_OFF_VERSION           0x14       /**< Offset to version. */
#define FDT_OFF_LAST_COMP_VERSION 0x18       /**< Offset to last compatible version. */
#define FDT_OFF_BOOT_CPUID_PHYS   0x1C       /**< Offset to boot CPU physical ID. */
#define FDT_OFF_SIZE_DT_STRINGS   0x20       /**< Offset to strings block size. */
#define FDT_OFF_SIZE_DT_STRUCT    0x24       /**< Offset to structure block size. */
#define FDT_COMPATIBLE_VERSION    17         /**< Minimum compatible FDT version. */
/** @} */

/**
 * @brief FDT token values.
 */
typedef enum : u32 {
	FDT_BEGIN_NODE = 0x1, /**< Begin node token. */
	FDT_END_NODE = 0x2,   /**< End node token. */
	FDT_PROP = 0x3,       /**< Property token. */
	FDT_NOP = 0x4,        /**< Nop token. */
	// Unused/Reserved 0x5 - 0x8
	FDT_END = 0x9 /**< End of structure block token. */
} fdt_token_t;

/// @}

#endif // !SRC_LIB_DT_DEFINES
