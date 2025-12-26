#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_props.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>

#include "dt_defines.h"

// Function to parse a block of properties starting at props_offset with props_size size in the FDT with fdt
// being a ptr to the start of the flattened device tree blob
// TODO: Change this to work in tandem with the static parse_subtree, maybe change names
/*
static buffer_t parse_props(const buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset) {
    u32 curr_offset = props_offset;

    dt_prop_t* head = nullptr;
    dt_prop_t** pp = &head;

    while (curr_offset < props_offset + props_size) {
        u32 tag;
        if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
            return nullptr;

        // Because of the separation of parsing properties and nodes, we don't want to parse non-properties
        if ((fdt_token_t)tag != FDT_PROP)
            break;

        curr_offset += 4;
        u32 len;
        u32 name_offset;

        if (!buffer_read_u32_be(*fdt_buf, curr_offset + 0, &len) ||
            !buffer_read_u32_be(*fdt_buf, curr_offset + 4, &name_offset))
            return nullptr;

        curr_offset += 8;

        const char* name;
        if (!buffer_read_cstring(*fdt_buf, str_offset + name_offset, &name))
            return nullptr;

        dt_prop_t* new_prop = dt_alloc(sizeof(*new_prop));
        if (!new_prop)
            return nullptr;

        *new_prop = (dt_prop_t){
            .name = name,
            .data = buffer_sub_buffer(*fdt_buf, curr_offset, len),
            .next_prop = nullptr,
        };

        if (!buffer_is_valid(new_prop->data))
            return nullptr;

        // At first it sets the head in the right place, then it sets the next_prop of the previous property to point to
        // the current property
        *pp = new_prop;
        pp = &new_prop->next_prop;

        curr_offset = align_u32(curr_offset + len, sizeof(u32));
    }
    return head;
}
*/

// Function to recursively parse a subtree at the given offset to find node by path and by name
// TODO: change all nullptr returns to invalid buffers, after finding the end correct node find the searched-after
// property and get it's buffer
static buffer_t parse_subtree(const buffer_t* fdt_buf, u32* offset, u32 max_offset, u32 str_offset, const char* path,
                              const char* p_name) {
	u32 curr_offset = *offset;

	u32 tag;
	if (!buffer_read_u32_be(*fdt_buf, curr_offset - 4, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return make_buffer(nullptr, 0);

	const char* name;
	if (!buffer_read_cstring(*fdt_buf, curr_offset, &name))
		return make_buffer(nullptr, 0);

	// After this point all reads from the buffer should be correct

	u32 name_len = strlen(name) + 1;

	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
			return make_buffer(nullptr, 0);

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset + 4, &p_len))
			return make_buffer(nullptr, 0);

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
			return make_buffer(nullptr, 0);

		curr_offset += 4;
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			u32 new_tag;
			if (!buffer_read_u32_be(*fdt_buf, curr_offset - 4, &new_tag) || (fdt_token_t)new_tag != FDT_BEGIN_NODE)
				return make_buffer(nullptr, 0);

			const char* name;
			if (!buffer_read_cstring(*fdt_buf, curr_offset, &name))
				return make_buffer(nullptr, 0);

			u32 name_len = strlen(name) + 1;

			curr_offset = align_u32(curr_offset + name_len, sizeof(u32));
			// Check if this is the child we're looking for
			dprintf("path: %s\n", path);
			dprintf("name: %s\n", name);
			size_t i = 0;
			bool wrong_child = false;
			while (i < name_len) {
				if (path[i] != name[i])
					wrong_child = true;
				i += 1;
			}

			if (!wrong_child) {
				// If this is the end child, process properties here
				return make_buffer((const void*)1, 0);
			} else {
				// Skip this node
				curr_offset += name_len;
				size_t depth = 1;
				// Depth to ignore all nested nodes
				while (curr_offset < max_offset && depth > 0) {
					u32 tag;
					if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
						return make_buffer(nullptr, 0);

					curr_offset += 4;

					switch (tag) {
					case FDT_BEGIN_NODE:
						depth += 1;
						const char* new_name;
						if (!buffer_read_cstring(*fdt_buf, curr_offset, &new_name))
							return make_buffer(nullptr, 0);

						u32 new_name_len = strlen(name) + 1;

						curr_offset = align_u32(curr_offset + new_name_len, sizeof(u32));

						break;

					case FDT_END_NODE: depth -= 1; break;

					case FDT_PROP: {
						u32 props_off = curr_offset;
						while (curr_offset < max_offset) {
							u32 tag;
							if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
								return make_buffer(nullptr, 0);

							if ((fdt_token_t)tag != FDT_PROP)
								break;

							u32 p_len;
							if (!buffer_read_u32_be(*fdt_buf, curr_offset + 4, &p_len))
								return make_buffer(nullptr, 0);

							curr_offset += 12; // Skip tag, length, name_offset
							curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
						}
						u32 props_len = curr_offset - props_off;

						curr_offset = align_u32(props_off + props_len, sizeof(u32));
						break;
					}

					case FDT_NOP: continue;

					case FDT_END: return make_buffer(nullptr, 0); // No such node found

					default: return make_buffer(nullptr, 0);      // Unknown element type (not defined in v17)
					}
				}
			}

			break;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return make_buffer(nullptr, 0); // No such node found

		default:                                      // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return make_buffer(nullptr, 0);
		}
	}

	DEBUG_PRINTF("Invalid FDT structure\n");
	return make_buffer(nullptr, 0);
}

// TODO: Part of this function could be generalized then used by the pareser and this without duplication
buffer_t dt_prop_get_immediate(const void* fdt, const char* path, const char* p_name) {
	if (!p_name)
		return make_buffer(nullptr, 0); // invalid buffer

	if (!fdt)
		return make_buffer(nullptr, 0);

	u32 magic = read_be32(fdt);
	if (magic != FDT_MAGIC)
		return make_buffer(nullptr, 0); // invalid buffer

	u32 fdt_size = read_be32((u8*)fdt + FDT_OFF_TOTAL_SIZE);

	buffer_t fdt_buf = make_buffer(fdt, fdt_size);

	if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return make_buffer(nullptr, 0); // invalid buffer

	u32 total_size;
	u32 struct_off;
	u32 strings_off;
	u32 struct_size;
	u32 fdt_version;
	if (!buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_VERSION, &fdt_version))
		return make_buffer(nullptr, 0);

	if (fdt_version > FDT_COMPATIBLE_VERSION)
		return make_buffer(nullptr, 0);

	if (struct_off + struct_size > total_size)
		return make_buffer(nullptr, 0);

	u32 offset = struct_off + 4;

	return parse_subtree(&fdt_buf, &offset, struct_off + struct_size, strings_off, path + 1, p_name);

	// PARSING

	return make_buffer(nullptr, 0);
}
