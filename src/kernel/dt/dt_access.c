#include <debug/debug_stdio.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt.h"
#include "dt_common.h"
#include "dt_defines.h"

const char* dt_walk(const char* path, const char** name, u32* name_len) {
	if (!path || path[0] != '/')
		return nullptr;

	const char* p = path;

	// Skip leading slashes
	while (*p == '/') p++;

	if (*p == '\0')
		return nullptr;

	const char* start = p;
	while (*p && *p != '/') p++;

	*name = start;
	*name_len = (u32)(p - start);

	while (*p == '/') p += 1;

	// If this is the last node, dt_walk will return the null terminator
	if (*p != '\0')
		p -= 1;

	return (*p) ? p : nullptr;
}

dt_node_t dt_get_node_in_subtree_by_path(const void* fdt, dt_node_t node, const char* node_path) {
	if (!node)
		node = dt_get_root();

	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = dt_skip_node_header(fdt, node);

	const char* next_node = nullptr;
	u32 node_name_len = 0;
	node_path = dt_walk(node_path, &next_node, &node_name_len);

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:

			const char* name;
			if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
				return 0;

			u32 name_len = strlen(name) + 1;

			curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

			// Not using strcmp due do the length mismatch
			bool wrong_child = false;

			// Check if this is the node we're looking for
			if (name_len - 1 != node_name_len) {
				wrong_child = true;
			}

			size_t i = 0;
			while (i < node_name_len && !wrong_child) {
				if (next_node[i] != name[i])
					wrong_child = true;

				i += 1;
			}

			if (!wrong_child) {
				// If no path left to follow, this is the sought node
				if (node_path == NULL) {
					return node_offset;
				} else {
					return dt_get_node_in_subtree_by_path(fdt, node_offset, node_path);
				}
			} else {
				curr_offset = dt_skip_nested_nodes(fdt, node_offset);
			}
			break;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}

	DEBUG_PRINTF("Invalid FDT structure\n");
	return 0;
}

dt_node_t dt_get_node_by_path(const void* fdt, const char* node_path) {
	return dt_get_node_in_subtree_by_path(fdt, (dt_node_t)0, node_path);
}

dt_node_t dt_get_node_child(const void* fdt, dt_node_t node) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = dt_skip_node_header(fdt, node);

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE: return node_offset;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}

	return 0;
}

dt_node_t dt_get_node_sibling(const void* fdt, dt_node_t node) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = dt_skip_node_header(fdt, node);

	bool not_nested = false;

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			if (not_nested == true)
				return node_offset;
			else {
				curr_offset = dt_skip_nested_nodes(fdt, node_offset);
				break;
			}

		case FDT_NOP: continue;

		case FDT_END_NODE: not_nested = true; break;
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}

	return 0;
}

bool dt_get_node_name(const void* fdt, dt_node_t node, char* out, u32 out_size) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	const char* name;
	if (!buffer_read_cstring(fdt_buf, node + sizeof(fdt_token_t), &name))
		return false;

	// Buffer too small
	if (strlen(name) + 1 > out_size)
		return false;

	strcpy(out, name);

	return true;
}

dt_prop_t dt_get_prop_by_name(const void* fdt, dt_node_t node, const char* prop_name) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	u32 str_offset = dt_get_strings_off();

	u32 curr_offset = node + sizeof(fdt_token_t);

	while (curr_offset < max_offset) {
		u32 tag;

		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;
		u32 prop_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);

		switch ((fdt_token_t)tag) {
		case FDT_PROP:
			u32 p_len;
			u32 name_offset;

			if (!buffer_read_u32_be(fdt_buf, curr_offset + 0, &p_len) ||
			    !buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &name_offset))
				return 0;

			curr_offset += 2 * sizeof(u32);

			const char* name;
			if (!buffer_read_cstring(fdt_buf, str_offset + name_offset, &name))
				return 0;

			if (!strcmp(name, prop_name))
				return prop_offset;

			curr_offset = align_u32(curr_offset + p_len, sizeof(u32));

			break;

		case FDT_NOP: continue;

		case FDT_BEGIN_NODE:
		case FDT_END_NODE:
		case FDT_END:        return 0; // No such node found
		}
	}
	return 0;
}

dt_prop_t dt_get_first_prop(const void* fdt, dt_node_t node) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = dt_skip_node_name(fdt, node);

	u32 tag;

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		switch ((fdt_token_t)tag) {
		case FDT_PROP: return curr_offset;

		case FDT_NOP: break;

		// Something else before the first prop
		default: return 0;
		}
	}

	return 0;
}

dt_prop_t dt_get_next_prop(const void* fdt, dt_prop_t prop) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = prop;

	u32 tag;
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
		return 0;

	// Check validity of prop
	if ((fdt_token_t)tag != FDT_PROP)
		return 0;

	u32 p_len;
	if (!buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &p_len))
		return 0;

	curr_offset += 3 * sizeof(u32); // Skip tag, length, name_offset
	curr_offset = align_u32(curr_offset + p_len, sizeof(u32));

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		switch ((fdt_token_t)tag) {
		case FDT_PROP: return curr_offset;

		case FDT_NOP: break;

		// Something else before the next prop
		default: return 0;
		}
	}

	return 0;
}

bool dt_get_prop_name(const void* fdt, dt_prop_t prop, char* out, u32 out_size) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 name_offset;
	if (!buffer_read_u32_be(fdt_buf, prop + 2 * sizeof(fdt_token_t), &name_offset))
		return false;

	const char* name;
	if (!buffer_read_cstring(fdt_buf, dt_get_strings_off() + name_offset, &name))
		return false;

	// Buffer too small
	if (strlen(name) + 1 > out_size)
		return false;

	strcpy(out, name);

	return true;
}

buffer_t dt_get_prop_buffer(const void* fdt, dt_prop_t prop) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 prop_len;
	if (!buffer_read_u32_be(fdt_buf, prop + sizeof(fdt_token_t), &prop_len))
		return make_buffer(nullptr, 0);

	return make_buffer(fdt + prop + 3 * sizeof(fdt_token_t), prop_len);
}
