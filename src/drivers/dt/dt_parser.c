#include "dt_parser.h"

#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_node.h"

// FDT token values
#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE   0x2
#define FDT_PROP       0x3
#define FDT_NOP        0x4
#define FDT_END        0x9

// Function to parse a block of properties starting at props_offset with props_size size in the FDT with fdt
// being a ptr to the start of the flattened device tree blob
struct dt_prop* parse_props(buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset) {
	u32 curr_offset = props_offset;

	struct dt_prop* head = nullptr;
	struct dt_prop** pp = &head;

	while (curr_offset < props_offset + props_size) {
		u32 tag;
		buffer_read_u32_be(fdt_buf, curr_offset, &tag);

		// Because of the separation of parsing properties and nodes, we don't want to parse non-properties
		if (tag != FDT_PROP)
			break;

		curr_offset += 4;
		u32 len;
		buffer_read_u32_be(fdt_buf, curr_offset, &len);
		curr_offset += 4;
		u32 name_offset;
		buffer_read_u32_be(fdt_buf, curr_offset, &name_offset);
		curr_offset += 4;
		struct dt_prop* new_prop = dt_alloc(sizeof(*new_prop));
		new_prop->data_length = len;
		new_prop->next_prop = nullptr;

		buffer_read_cstring(fdt_buf, str_offset + name_offset, &new_prop->name);
		// sbi_puts(new_prop->name);
		// sbi_puts("\n");

		// Forced
		new_prop->value = (const u8*)fdt_buf->data + curr_offset;

		// At first it sets the head in the right place, then it sets the next_prop of the previous property to point to
		// the current property
		*pp = new_prop;
		pp = &new_prop->next_prop;

		curr_offset = alignN(curr_offset + len, 4);
	}
	return head;
}

// Function to recursively parse a subtree at the given offset
struct dt_node* parse_subtree(buffer_t* fdt_buf, u32* offset, u32 max_offset, u32 str_offset, struct dt_node* parent) {
	struct dt_node* node = dt_alloc(sizeof(*node));

	if (!node)
		return nullptr;

	node->parent = parent;
	node->first_child = nullptr;
	node->next_sibling = nullptr;
	node->props = nullptr;
	node->phandle = 0;

	u32 curr_offset = *offset;

	u32 tag;

	if (buffer_read_u32_be(fdt_buf, curr_offset - 4, &tag) != BUFFER_OK || tag != FDT_BEGIN_NODE)
		return nullptr;

	const char* name;
	if (buffer_read_cstring(fdt_buf, curr_offset, &name) != BUFFER_OK)
		return nullptr;

	// After this point all reads from the buffer should be correct

	u32 name_len = strlen(name) + 1;

	node->name = name;

	curr_offset = alignN(curr_offset + name_len, 4);

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		buffer_read_u32_be(fdt_buf, curr_offset, &tag);
		if (tag != FDT_PROP)
			break;

		u32 p_len;
		buffer_read_u32_be(fdt_buf, curr_offset + 4, &p_len);

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = alignN(curr_offset + p_len, 4);
	}
	u32 props_len = curr_offset - props_off;

	node->props = parse_props(fdt_buf, props_off, props_len, str_offset);

	curr_offset = alignN(props_off + props_len, 4);

	while (curr_offset < max_offset) {
		u32 tag;
		buffer_read_u32_be(fdt_buf, curr_offset, &tag);

		curr_offset += 4;
		switch (tag) {
		case FDT_BEGIN_NODE:
			struct dt_node* child = parse_subtree(fdt_buf, &curr_offset, max_offset, str_offset, node);

			if (child) {
				if (!node->first_child)
					node->first_child = child;
				else {
					struct dt_node* next = node->first_child;
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
