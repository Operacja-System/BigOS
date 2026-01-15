#include "dt_common.h"

#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt.h"
#include "dt_defines.h"

u32 dt_skip_node_name(const void* fdt, dt_node_t node) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = node;

	u32 tag;

	// Check validity of node
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return 0;

	curr_offset += sizeof(fdt_token_t);

	const char* name;
	if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
		return 0;

	u32 name_len = strlen(name) + 1;

	// Skip name of node
	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	return curr_offset;
}

u32 dt_skip_node_properties(const void* fdt, u32 offset) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = offset;

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	// Skip properties of node
	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &p_len))
			return 0;

		curr_offset += 3 * sizeof(u32); // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	return curr_offset;
}

u32 dt_skip_node_header(const void* fdt, dt_node_t node) {
	u32 curr_offset = node;

	u32 name_skip = dt_skip_node_name(fdt, curr_offset);

	if (name_skip == 0)
		return 0;

	u32 prop_skip = dt_skip_node_properties(fdt, name_skip);

	// If skip dt_skip_node_properties breaks, prop_skip will return 0
	return prop_skip;
}

u32 dt_skip_nested_nodes(const void* fdt, dt_node_t nested_node) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = nested_node;

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	// To actually go in for the first nested node
	bool first = true;
	// Skip this node
	size_t depth = 0;
	// Depth to ignore all nested nodes
	while ((curr_offset < max_offset && depth > 0) || first) {
		first = false;

		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		switch (tag) {
		case FDT_BEGIN_NODE:

			depth += 1;
			curr_offset = dt_skip_node_name(fdt, curr_offset);

			if (curr_offset == 0)
				return 0;

			break;

		case FDT_END_NODE:
			curr_offset += sizeof(fdt_token_t);
			depth -= 1;
			break;

		case FDT_PROP:
			curr_offset = dt_skip_node_properties(fdt, curr_offset);

			if (curr_offset == 0)
				return 0;

			break;

		case FDT_NOP: curr_offset += sizeof(fdt_token_t); break;

		case FDT_END:
		default: // Unknown element type (not defined in v17)
			return 0;
		}
	}

	return curr_offset;
}
