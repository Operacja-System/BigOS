/**
 * @ingroup dt_internal
 * @file dt_common.h
 * @brief Common device tree parsing utilities.
 */

#ifndef LIBBOOT_DT_COMMON
#define LIBBOOT_DT_COMMON

#include <libboot/dt/dt.h>
#include <libcore/error.h>
#include <libcore/types.h>

// Offsets are of type u32

/// @addtogroup dt_internal
/// @ingroup libboot_dt
/// @{

/**
 * @brief Skip node's tag and name and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @param[out] offsetOUT First aligned offset after the @p node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_skip_node_name(const fdt_t* fdt, dt_node_t node, u32* offsetOUT);

/**
 * @brief Skip node's properties starting at offset (if they exist) and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param offset The offset to start skipping properties from.
 * @param[out] offsetOUT First aligned offset after properties at @p offset.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_skip_node_properties(const fdt_t* fdt, u32 offset, u32* offsetOUT);

/**
 * @brief Skip node's tag, name and properties and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @param[out] offsetOUT First aligned offset after the @p node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_skip_node_header(const fdt_t* fdt, dt_node_t node, u32* offsetOUT);

/**
 * @brief Skip an overarching node's nested nodes starting at the start of the first nested node.
 * @param fdt Pointer to the flattened device tree.
 * @param nested_node The first nested node to start skipping from.
 * @param[out] nodeOUT First non-nested node after the @p nested_node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p nested_node
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_skip_nested_nodes(const fdt_t* fdt, dt_node_t nested_node, dt_node_t* nodeOUT);

/// @}

#endif // !LIBBOOT_DT_COMMON
