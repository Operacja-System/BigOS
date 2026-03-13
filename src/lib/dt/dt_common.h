/**
 * @file dt_common.h
 * @brief Common device tree parsing utilities.
 */

#ifndef SRC_LIB_DT_COMMON
#define SRC_LIB_DT_COMMON

#include <dt/dt.h>
#include <stdbigos/types.h>

// Offsets are of type u32

/**
 * @ingroup dt
 * @brief Skip node's tag and name and return first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @return >0 if success, 0 if error.
 */
u32 dt_skip_node_name(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Skip node's properties starting at offset (if they exist) and return first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param offset The offset to start skipping properties from.
 * @return >0 if success, 0 if error.
 */
u32 dt_skip_node_properties(const fdt_t* fdt, u32 offset);

/**
 * @ingroup dt
 * @brief Skip node's tag, name and properties and return first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @return >0 if success, 0 if error.
 */
u32 dt_skip_node_header(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Skip an overarching node's nested nodes starting at the start of the first nested node.
 * @param fdt Pointer to the flattened device tree.
 * @param nested_node The first nested node to start skipping from.
 * @return >0 if success, 0 if error.
 */
u32 dt_skip_nested_nodes(const fdt_t* fdt, dt_node_t nested_node);

#endif // !SRC_LIB_DT_COMMON
