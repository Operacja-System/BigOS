#include "dt_parser.h"

#include <debug/debug_stdio.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_node.h"

// FDT token values
typedef enum : u32 {
	FDT_BEGIN_NODE = 0x1,
	FDT_END_NODE = 0x2,
	FDT_PROP = 0x3,
	FDT_NOP = 0x4,
	// Unused/Reserved 0x5 - 0x8
	FDT_END = 0x9
} fdt_token_t;

// Function to parse a block of properties starting at props_offset with props_size size in the FDT with fdt
// being a ptr to the start of the flattened device tree blob
dt_prop_t* parse_props(const buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset) {
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

// Function to recursively parse a subtree at the given offset
dt_node_t* parse_subtree(const buffer_t* fdt_buf, u32* offset, u32 max_offset, u32 str_offset, dt_node_t* parent) {
	dt_node_t* node = dt_alloc(sizeof(*node));

	if (!node)
		return nullptr;

	node->parent = parent;
	node->first_child = nullptr;
	node->next_sibling = nullptr;
	node->props = nullptr;
	node->phandle = 0;

	u32 curr_offset = *offset;

	u32 tag;
	if (!buffer_read_u32_be(*fdt_buf, curr_offset - 4, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return nullptr;

	const char* name;
	if (!buffer_read_cstring(*fdt_buf, curr_offset, &name))
		return nullptr;

	// After this point all reads from the buffer should be correct

	u32 name_len = strlen(name) + 1;
	node->name = name;

	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
			return nullptr;

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset + 4, &p_len))
			return nullptr;

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	node->props = parse_props(fdt_buf, props_off, props_len, str_offset);

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	dt_node_t** p_next_sibling = &node->first_child;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
			return nullptr;

		curr_offset += 4;
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			dt_node_t* child = parse_subtree(fdt_buf, &curr_offset, max_offset, str_offset, node);

			if (child) {
				*p_next_sibling = child;
				p_next_sibling = &child->next_sibling;
			}
			break;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      *offset = curr_offset; return node;

		default: // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return nullptr;
		}
	}

	DEBUG_PRINTF("Invalid FDT structure\n");
	return nullptr;
};
