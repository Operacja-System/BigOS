#include "dt_parser.h"

#include <drivers/dt/dt_node.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"

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
		if (buffer_read_u32_be(*fdt_buf, curr_offset, &tag) != BUF_ERR_OK)
			return nullptr;

		// Because of the separation of parsing properties and nodes, we don't want to parse non-properties
		if ((fdt_token_t)tag != FDT_PROP)
			break;

		curr_offset += 4;
		u32 len;
		if (buffer_read_u32_be(*fdt_buf, curr_offset, &len) != BUF_ERR_OK)
			return nullptr;
		curr_offset += 4;
		u32 name_offset;
		if (buffer_read_u32_be(*fdt_buf, curr_offset, &name_offset) != BUF_ERR_OK)
			return nullptr;
		curr_offset += 4;
		dt_prop_t* new_prop = dt_alloc(sizeof(*new_prop));
		new_prop->data_length = len;
		new_prop->next_prop = nullptr;
		if (buffer_read_cstring(*fdt_buf, str_offset + name_offset, &new_prop->name) != BUF_ERR_OK)
			return nullptr;
		// sbi_puts(new_prop->name);
		// sbi_puts("\n");

		// Forced
		new_prop->value = (const u8*)fdt_buf->data + curr_offset;

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

	if (!node)
		return nullptr;

	node->parent = parent;
	node->first_child = nullptr;
	node->next_sibling = nullptr;
	node->props = nullptr;
	node->phandle = 0;

	u32 curr_offset = *offset;

	u32 tag;
	if (buffer_read_u32_be(*fdt_buf, curr_offset - 4, &tag) != BUF_ERR_OK || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return nullptr;

	const char* name;
	if (buffer_read_cstring(*fdt_buf, curr_offset, &name) != BUF_ERR_OK)
		return nullptr;

	// After this point all reads from the buffer should be correct

	if (!name)
		return nullptr;

	const char* name;
	if (buffer_read_cstring(*fdt_buf, curr_offset, &name) != BUFFER_ERROR_OK)
		return nullptr;

	// After this point all reads from the buffer should be correct

	u32 name_len = strlen(name) + 1;

	node->name = name;

	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (buffer_read_u32_be(*fdt_buf, curr_offset, &tag) != BUF_ERR_OK)
			return nullptr;

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (buffer_read_u32_be(*fdt_buf, curr_offset + 4, &p_len) != BUF_ERR_OK)
			return nullptr;

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	node->props = parse_props(fdt_buf, props_off, props_len, str_offset);

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	curr_offset = alignN(props_off + props_len, 4);

	while (curr_offset < max_offset) {
		u32 tag;
		if (buffer_read_u32_be(*fdt_buf, curr_offset, &tag) != BUF_ERR_OK)
			return nullptr;

		curr_offset += 4;
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			dt_node_t* child = parse_subtree(fdt_buf, &curr_offset, max_offset, str_offset, node);

			if (child) {
				if (!node->first_child)
					node->first_child = child;
				else {
					dt_node_t* next = node->first_child;
					while (next->next_sibling) {
						next = next->next_sibling;
					}
					next->next_sibling = child;
				}
			}
			break;

		case FDT_END_NODE:

			*offset = curr_offset; //
			return node;

		case FDT_NOP:              //
			continue;

		case FDT_END:              //
			*offset = curr_offset;
			return node;

		default: // Maybe panic?
			*offset = curr_offset;
			return node;
		}
	}

	*offset = curr_offset;
	return node;
};
// Note: dt_init.c must parse FDT header to get initial offset and size,
// then call parse_subtree(fdt_blob, &offset, struct_off + struct_size, NULL)
