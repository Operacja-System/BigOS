/**
 * @ingroup dt
 * @file dt.h
 * @brief Device tree access interface.
 */

#ifndef _DT_
#define _DT_

// TODO: Change return values of functions that return e.g. no next properties and errors as the same

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

/**
 * @ingroup dt
 * @brief A node is an offset from the start of fdt with FDT_BEGIN_NODE.
 */
typedef u32 dt_node_t;

/**
 * @ingroup dt
 * @brief A property is an offset from the start of fdt with FDT_PROP.
 */
typedef u32 dt_prop_t;

/**
 * @ingroup dt
 * @brief Flattened device tree structure.
 */
typedef struct {
	buffer_t fdt_buffer; /**< Buffer containing the FDT data. */
	dt_node_t root_node; /**< Root node offset. */
	u32 total_size;      /**< Total size of the FDT. */
	u32 struct_off;      /**< Structure block offset. */
	u32 strings_off;     /**< Strings block offset. */
	u32 struct_size;     /**< Structure block size. */
	u32 fdt_version;     /**< FDT version. */
} fdt_t;

/**
 * @ingroup dt
 * @brief Read the header at fdt and set fields of obj.
 * @param fdt Pointer to the flattened device tree data.
 * @param obj Pointer to the fdt_t structure to initialize.
 * @return 0 if success, <0 if error. If an error occurs, fdt_buffer of obj is set to an invalid buffer.
 */
int dt_init(const void* fdt, fdt_t* obj);

/**
 * @ingroup dt
 * @brief Reset properties of obj.
 * @param obj Pointer to the fdt_t structure to reset.
 */
void dt_reset(fdt_t* obj);

/**
 * @ingroup dt
 * @brief Get a node in a subtree of fdt, leave as 0 for global search.
 * @param fdt Pointer to the fdt object.
 * @param node The node of which subtree to search, leave as 0 for global search.
 * @param node_path Path to the node.
 * @return >0 if success, 0 if error.
 */
dt_node_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path);

/**
 * @ingroup dt
 * @brief Get a node globally in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node_path Path to the node.
 * @return >0 if success, 0 if error.
 */
dt_node_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path);

/**
 * @ingroup dt
 * @brief Get a node's first child in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The parent node.
 * @return 0 if no children exist or error, >0 if success.
 */
dt_node_t dt_get_node_child(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Get a node's next sibling in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the sibling of.
 * @return 0 if no siblings exist or error, >0 if success.
 */
dt_node_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Get a node's name as a buffer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the name of.
 * @return Valid buffer if success, invalid buffer if error.
 */
buffer_t dt_get_node_name(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Get a node's name as a pointer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the name of.
 * @return Pointer to the node name.
 */
const char* dt_get_node_name_ptr(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Get a node's prop_name property in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to search in.
 * @param prop_name Name of the property.
 * @return >0 if success, 0 if error.
 */
dt_prop_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name);

/**
 * @ingroup dt
 * @brief Get a node's first property in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the first property of.
 * @return 0 if no properties exist or error, >0 if success.
 */
dt_prop_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node);

/**
 * @ingroup dt
 * @brief Get a node's next property after prop in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The current property.
 * @return 0 if no next properties exist or error, >0 if success.
 */
dt_prop_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop);

/**
 * @ingroup dt
 * @brief Get a prop's name as a buffer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the name of.
 * @return Valid buffer if success, invalid buffer if error.
 */
buffer_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop);

/**
 * @ingroup dt
 * @brief Get a prop's name as a pointer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the name of.
 * @return Pointer to the property name.
 */
const char* dt_get_prop_name_ptr(const fdt_t* fdt, dt_prop_t prop);

/**
 * @ingroup dt
 * @brief Get a buffer for prop's data.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the data of.
 * @return Buffer containing the property data.
 */
buffer_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop);

#endif // !_DT_
