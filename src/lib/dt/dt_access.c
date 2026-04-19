#include <debug/debug_stdio.h>
#include <dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

#include "dt_common.h"
#include "dt_defines.h"

static buffer_t buffer_strtok(const char** path, const char* sep) {
	if (!path)
		return make_buffer(nullptr, 0);

	const char* p = *path;
	p += strspn(p, sep);

	if (*p == '\0') {
		*path = nullptr;
		return make_buffer(nullptr, 0);
	}

	size_t sz = strcspn(p, sep);

	*path = p + sz;
	return make_buffer(p, sz);
}

error_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path, dt_node_t* nodeOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	if (!node && fdt->root_node)
		node = fdt->root_node;

	while (true) {
		buffer_t segment_name = buffer_strtok(&node_path, "/");
		if (!buffer_is_valid(segment_name)) {
			*nodeOUT = node;
			return ERR_NONE;
		}

		error_t error = dt_get_node_child(fdt, node, &node);

		if (error != ERR_NONE)
			return error;

		bool found = false;
		while (true) {
			buffer_t child_name;

			error = dt_get_node_name(fdt, node, &child_name);

			if (error != ERR_NONE)
				return error;

			if (buffer_equal(child_name, segment_name)) {
				found = true;
				break;
			}

			error = dt_get_node_sibling(fdt, node, &node);

			if (error != ERR_NONE)
				return error;
		}

		if (!found)
			return ERR_NOT_FOUND;
	}
}

error_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path, dt_node_t* nodeOUT) {
	return dt_get_node_in_subtree_by_path(fdt, (dt_node_t)0, node_path, nodeOUT);
}

error_t dt_get_node_child(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset;
	error_t error = dt_skip_node_header(fdt, node, &curr_offset);

	if (error != ERR_NONE)
		return error;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return ERR_NOT_VALID;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE: *nodeOUT = node_offset; return ERR_NONE;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return ERR_NOT_FOUND; // No such node found

		default:                            // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return ERR_NOT_VALID;
		}
	}

	return ERR_OUT_OF_BOUNDS;
}

error_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset;
	error_t error = dt_skip_node_header(fdt, node, &curr_offset);

	if (error != ERR_NONE)
		return error;

	bool not_nested = false;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return ERR_NOT_VALID;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			if (not_nested) {
				*nodeOUT = node_offset;
				return ERR_NONE;
			}

			error = dt_skip_nested_nodes(fdt, node_offset, &curr_offset);
			if (error != ERR_NONE)
				return error;

			break;

		case FDT_NOP: continue;

		case FDT_END_NODE: not_nested = true; break;
		case FDT_END:      return ERR_NOT_FOUND; // No such node found

		default:                            // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return ERR_NOT_VALID;
		}
	}

	return ERR_OUT_OF_BOUNDS;
}

error_t dt_get_node_name(const fdt_t* fdt, dt_node_t node, buffer_t* bufOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = node;

	u32 tag;

	// Check validity of node
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
		return ERR_NOT_VALID;

	if ((fdt_token_t)tag != FDT_BEGIN_NODE)
		return ERR_BAD_ARG;

	const char* name;
	u64 name_len;
	if (!buffer_read_cstring_len(fdt_buf, curr_offset + sizeof(fdt_token_t), &name, &name_len))
		return ERR_NOT_VALID;

	*bufOUT = buffer_sub_buffer(fdt_buf, curr_offset + sizeof(fdt_token_t), name_len);

	return ERR_NONE;
}

error_t dt_get_node_name_ptr(const fdt_t* fdt, dt_node_t node, const char** ptrOUT) {
	buffer_t b;
	error_t error = dt_get_node_name(fdt, node, &b);

	if (error != ERR_NONE)
		return error;

	*ptrOUT = b.data;

	return ERR_NONE;
}

error_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name, dt_prop_t* propOUT) {
	if (fdt == nullptr || !prop_name)
		return ERR_BAD_ARG;

	buffer_t name = make_buffer(prop_name, strlen(prop_name));
	dt_prop_t prop;
	error_t error = dt_get_first_prop(fdt, node, &prop);

	if (error != ERR_NONE)
		return error;

	while (true) {
		buffer_t child_name;
		error_t error = dt_get_prop_name(fdt, prop, &child_name);

		if (error != ERR_NONE)
			return error;

		if (buffer_equal(child_name, name)) {
			*propOUT = prop;
			return ERR_NONE;
		}

		error = dt_get_next_prop(fdt, prop, &prop);

		if (error != ERR_NONE) {
			return error;
		}
	}
}

error_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node, dt_prop_t* propOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset;
	error_t error = dt_skip_node_name(fdt, node, &curr_offset);

	if (error != ERR_NONE)
		return error;

	u32 tag;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return ERR_NOT_VALID;

		switch ((fdt_token_t)tag) {
		case FDT_PROP: *propOUT = curr_offset; return ERR_NONE;

		case FDT_NOP: break;

		case FDT_END_NODE:
		case FDT_BEGIN_NODE: return ERR_NOT_FOUND;

		// Something else before the first prop
		default: return ERR_NOT_VALID;
		}
	}

	return ERR_NOT_FOUND;
}

error_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop, dt_prop_t* propOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = prop;

	u32 tag;
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
		return ERR_NOT_VALID;

	// Check validity of prop
	if ((fdt_token_t)tag != FDT_PROP)
		return ERR_BAD_ARG;

	u32 p_len;
	if (!buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &p_len)) {
		return ERR_NOT_VALID;
	}

	curr_offset += 3 * sizeof(u32); // Skip tag, length, name_offset
	curr_offset = align_u32(curr_offset + p_len, sizeof(u32));

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag)) {
			return ERR_NOT_VALID;
		}

		switch ((fdt_token_t)tag) {
		case FDT_PROP: *propOUT = curr_offset; return ERR_NONE;

		case FDT_NOP: break;

		case FDT_END_NODE:
		case FDT_BEGIN_NODE: return ERR_NOT_FOUND;

		// Something else before the next prop
		default: return ERR_NOT_VALID;
		}
	}

	return ERR_OUT_OF_BOUNDS;
}

error_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 name_offset;
	if (!buffer_read_u32_be(fdt_buf, prop + 2 * sizeof(fdt_token_t), &name_offset))
		return ERR_NOT_VALID;

	const char* name;
	u64 name_len;
	if (!buffer_read_cstring_len(fdt_buf, fdt->strings_off + name_offset, &name, &name_len))
		return ERR_NOT_VALID;

	*bufOUT = buffer_sub_buffer(fdt_buf, fdt->strings_off + name_offset, name_len);

	return ERR_NONE;
}

error_t dt_get_prop_name_ptr(const fdt_t* fdt, dt_prop_t prop, const char** ptrOUT) {
	buffer_t buf;
	error_t error = dt_get_prop_name(fdt, prop, &buf);

	if (error != ERR_NONE)
		return error;

	*ptrOUT = buf.data;
	return ERR_NONE;
}

error_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 prop_len;
	if (!buffer_read_u32_be(fdt_buf, prop + sizeof(fdt_token_t), &prop_len))
		return ERR_NOT_VALID;

	*bufOUT = buffer_sub_buffer(fdt_buf, prop + 3 * sizeof(fdt_token_t), prop_len);

	return ERR_NONE;
}

error_t dt_get_rsv_mem_entry(const fdt_t* fdt, u32 index, fdt_rsv_entry* entryOUT) {
	if (fdt == nullptr)
		return ERR_BAD_ARG;

	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_index = 0;

	u32 max_offset = fdt->struct_off;

	fdt_rsv_entry entry;

	for (u32 curr_offset = fdt->rsvmap_off; curr_offset + 2 * sizeof(u64) <= max_offset;
	     curr_offset += 2 * sizeof(u64)) {
		if (!buffer_read_u64_be(fdt_buf, curr_offset, &entry.address) ||
		    !buffer_read_u64_be(fdt_buf, curr_offset + sizeof(u64), &entry.size)) {
			return ERR_NOT_VALID;
		}

		if (entry.address == 0 && entry.size == 0) {
			return ERR_OUT_OF_BOUNDS;
		}

		if (curr_index == index) {
			*entryOUT = entry;
			return ERR_NONE;
		}

		curr_index += 1;
	}

	return ERR_NOT_VALID;
}
