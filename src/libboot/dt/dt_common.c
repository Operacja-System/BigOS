#include "dt_common.h"

#include <libboot/dt/dt.h>
#include <libcore/bitutils.h>
#include <libcore/buffer.h>
#include <libcore/string.h>
#include <libcore/types.h>

#include "dt_defines.h"

error_t dt_skip_node_name(const fdt_t* fdt, dt_node_t node, u32* offsetOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = node;

	u32 tag;

	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
		return ERR_NOT_VALID;

	// Check validity of node
	if ((fdt_token_t)tag != FDT_BEGIN_NODE)
		return ERR_BAD_ARG;

	curr_offset += sizeof(fdt_token_t);

	const char* name;
	if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
		return ERR_NOT_VALID;

	u32 name_len = strlen(name) + 1;

	// Skip name of node
	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	*offsetOUT = curr_offset;

	return ERR_NONE;
}

error_t dt_skip_node_properties(const fdt_t* fdt, u32 offset, u32* offsetOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = offset;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	// Skip properties of node
	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return ERR_NOT_VALID;

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &p_len))
			return ERR_NOT_VALID;

		curr_offset += 3 * sizeof(u32); // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	*offsetOUT = curr_offset;

	return ERR_NONE;
}

error_t dt_skip_node_header(const fdt_t* fdt, dt_node_t node, u32* offsetOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	u32 curr_offset = node;

	error_t error = dt_skip_node_name(fdt, curr_offset, &curr_offset);

	if (error != ERR_NONE)
		return error;

	error = dt_skip_node_properties(fdt, curr_offset, &curr_offset);

	if (error != ERR_NONE)
		return error;

	*offsetOUT = curr_offset;

	return ERR_NONE;
}

error_t dt_skip_nested_nodes(const fdt_t* fdt, dt_node_t nested_node, dt_node_t* nodeOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = nested_node;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	// To actually go in for the first nested node
	bool first = true;
	// Skip this node
	size_t depth = 0;
	// Depth to ignore all nested nodes
	while ((curr_offset < max_offset && depth > 0) || first) {
		first = false;

		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return ERR_NOT_VALID;

		switch (tag) {
		case FDT_BEGIN_NODE:

			depth += 1;

			error_t error = dt_skip_node_name(fdt, curr_offset, &curr_offset);

			if (error != ERR_NONE)
				return error;

			break;

		case FDT_END_NODE:
			curr_offset += sizeof(fdt_token_t);
			depth -= 1;
			break;

		case FDT_PROP:
			error = dt_skip_node_properties(fdt, curr_offset, &curr_offset);

			if (error != ERR_NONE)
				return error;

			break;

		case FDT_NOP: curr_offset += sizeof(fdt_token_t); break;

		case FDT_END:
		default: // Unknown element type (not defined in v17)
			return ERR_NOT_VALID;
		}
	}

	*nodeOUT = curr_offset;

	return ERR_NONE;
}
