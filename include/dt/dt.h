/**
 * @ingroup dt
 * @file dt.h
 * @brief Device tree access interface.
 */

#ifndef DT_DT
#define DT_DT

#include <stdbigos/buffer.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

/// @addtogroup dt
/// @{

/**
 * @brief A node is an offset from the start of fdt with FDT_BEGIN_NODE.
 */
typedef u32 dt_node_t;

/**
 * @brief A property is an offset from the start of fdt with FDT_PROP.
 */
typedef u32 dt_prop_t;

/**
 * @brief Flattened device tree structure.
 */
typedef struct {
	buffer_t fdt_buffer; /**< Buffer containing the FDT data. */
	dt_node_t root_node; /**< Root node offset. */
	u32 total_size;      /**< Total size of the FDT. */
	u32 struct_off;      /**< Structure block offset. */
	u32 rsvmap_off;      /**< Memory reservation map offset. */
	u32 strings_off;     /**< Strings block offset. */
	u32 struct_size;     /**< Structure block size. */
	u32 fdt_version;     /**< FDT version. */
} fdt_t;

/**
 * @brief Reserved memory entry structure.
 */
typedef struct {
	u64 address; /**< Entry address. */
	u64 size;    /**< Entry size. */
} fdt_rsv_entry;

/**
 * @brief Read the header at fdt and set fields of obj.
 * @param fdt Pointer to the flattened device tree data.
 * @param obj Pointer to the fdt_t structure to initialize.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid or unsupported
 * @note If an error occurs, fdt_buffer of obj is set to an invalid buffer.
 */
error_t dt_init(const void* fdt, fdt_t* obj);

/**
 * @brief Reset properties of obj.
 * @param obj Pointer to the fdt_t structure to reset.
 */
void dt_reset(fdt_t* obj);

/**
 * @brief Get a node in a subtree of fdt, leave as 0 for global search.
 * @param fdt Pointer to the fdt object.
 * @param node The node of which subtree to search, leave as 0 for global search.
 * @param node_path Path to the node.
 * @param[out] nodeOUT Found node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no node found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(4)]]
error_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path, dt_node_t* nodeOUT);

/**
 * @brief Get a node globally in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node_path Path to the node.
 * @param[out] nodeOUT Found node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no node found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(3)]]
error_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path, dt_node_t* nodeOUT);

/**
 * @brief Get a node's first child in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The parent node.
 * @param[out] nodeOUT Child node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no node found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(3)]]
error_t dt_get_node_child(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT);

/**
 * @brief Get a node's next sibling in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the sibling of.
 * @param[out] nodeOUT Sibling node.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no node found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(3)]]
error_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT);

/**
 * @brief Get a node's name as a buffer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the name of.
 * @param[out] bufOUT Buffer with the name.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_get_node_name(const fdt_t* fdt, dt_node_t node, buffer_t* bufOUT);

/**
 * @brief Get a node's name as a pointer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the name of.
 * @param[out] ptrOUT Pointer to the name.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_get_node_name_ptr(const fdt_t* fdt, dt_node_t node, const char** ptrOUT);

/**
 * @brief Get a node's prop_name property in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to search in.
 * @param prop_name Name of the property.
 * @param[out] propOUT Property.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no prop found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(4)]]
error_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name, dt_prop_t* propOUT);

/**
 * @brief Get a node's first property in fdt.
 * @param fdt Pointer to the fdt object.
 * @param node The node to get the first property of.
 * @param[out] propOUT First property.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p node
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no prop found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(3)]]
error_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node, dt_prop_t* propOUT);

/**
 * @brief Get a node's next property after prop in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The current property.
 * @param[out] propOUT Next property.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p prop
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_NOT_FOUND if no prop found
 * @retval ERR_OUT_OF_BOUNDS if search went out of bounds
 */
[[gnu::nonnull(3)]]
error_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop, dt_prop_t* propOUT);

/**
 * @brief Get a prop's name as a buffer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the name of.
 * @param[out] bufOUT Buffer with the name.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p prop
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT);

/**
 * @brief Get a prop's name as a pointer in fdt.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the name of.
 * @param[out] ptrOUT Pointer to the name.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args or misaligned @p prop
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_get_prop_name_ptr(const fdt_t* fdt, dt_prop_t prop, const char** ptrOUT);

/**
 * @brief Get a buffer for prop's data.
 * @param fdt Pointer to the fdt object.
 * @param prop The property to get the data of.
 * @param[out] bufOUT Buffer with the data.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 */
[[gnu::nonnull(3)]]
error_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT);

/**
 * @brief Get a reserved memory entry at index from the Memory Reservation Block.
 * @param fdt Pointer to the fdt object.
 * @param index Index of the entry to get.
 * @param[out] entryOUT Entry at index.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG on nullptr args
 * @retval ERR_NOT_VALID if the FDT is invalid
 * @retval ERR_OUT_OF_BOUNDS if index is out of bounds or the Memory Reservation Block is malformed
 */
[[gnu::nonnull(3)]]
error_t dt_get_rsv_mem_entry(const fdt_t* fdt, u32 index, fdt_rsv_entry* entryOUT);

/// @}

#endif // !DT_DT
