#include <drivers/dt/dt_alloc.h>
#include <drivers/dt/dt_node.h>
#include <drivers/dt/dt_parser.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

// FDT token values
#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE   0x2
#define FDT_PROP       0x3
#define FDT_NOP        0x4
#define FDT_END        0x9

// The FDT is in big-endian and risc-v is in little-endian, so we need a function to read big-endian u32 values
u32 read_be32(const void* addr) {
	const u8* bytes = (const u8*)addr;
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
};

void test() {
	;
}
// Function to parse a block of properties starting at props_offset with props_size size in the FDT with fdt
// being a ptr to the start of the flattened device tree blob
struct dt_prop* parse_props(const void* fdt, u32 props_offset, u32 props_size) {
	u32 curr_offset = props_offset;
	const u32* fdt_u32 = (const u32*)fdt;
	struct dt_prop* head = ((void*)0);
	struct dt_prop** pp = &head;

	while (curr_offset < props_offset + props_size) {
		u32 tag = read_be32(fdt_u32 + curr_offset);
		curr_offset += 4;

		// Because of the separation of parsing properties and nodes, we don't want to parse non-properties
		if (tag != FDT_PROP)
			break;

		u32 len = read_be32(fdt_u32 + curr_offset);
		curr_offset += 4;
		u32 name_offset = read_be32(fdt_u32 + curr_offset);
		curr_offset += 4;

		struct dt_prop* new_prop = dt_alloc(sizeof(*new_prop));
		new_prop->data_length = len;
		new_prop->next_prop = ((void*)0);
		// TODO: check if this works, should be available after
		new_prop->name = (const char*)(fdt_u32 + name_offset);

		void* value_buffer = dt_alloc(len);

		if (value_buffer)
			memcpy(value_buffer, fdt_u32, curr_offset, len);

		new_prop->value = value_buffer;

		// At first it sets the head in the right place, then it sets the next_prop of the previous property to point to
		// the current property
		*pp = new_prop;
		pp = &new_prop->next_prop;

		curr_offset = align4(curr_offset + len);
	}

	return head;
}

// Function to recursively parse a subtree at the given offset
struct dt_node* parse_subtree(const void* fdt, u32* offset, u32 max_offset, struct dt_node* parent) {
	const u32* fdt_u32 = (const u32*)fdt;
	struct dt_node* node = dt_alloc(sizeof(*node));

	node->parent = parent;
	node->first_child = ((void*)0);
	node->next_sibling = ((void*)0);
	node->props = ((void*)0);
	node->phandle = 0;

	u32 curr_offset = *offset;

	const char* name = (const char*)(fdt_u32 + curr_offset);
	// TODO: check if this works
	u32 name_len = strlen(name) + 1;

	char* namebuf = dt_alloc(name_len);
	strncpy(namebuf, name, name_len);
	node->name = namebuf;

	curr_offset = align4(curr_offset + name_len);

	u32 prop_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag = read_be32(fdt_u32 + curr_offset);
		if (tag != FDT_PROP)
			break;
		u32 p_len = read_be32(fdt_u32 + curr_offset + 4);
		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align4(curr_offset + p_len);
	}
	u32 prop_len = curr_offset - prop_off;
	node->props = parse_props(fdt, prop_off, prop_len);

	curr_offset = align4(prop_off + prop_len);

	while (curr_offset < max_offset) {
		u32 tag = read_be32(fdt_u32 + curr_offset);
		curr_offset += 4;
		switch (tag) {
		case FDT_BEGIN_NODE:
			struct dt_node* child = parse_subtree(fdt, &curr_offset, max_offset, node);
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

		case FDT_END:
			*offset = curr_offset;
			return node;
			break;

		default: // Maybe panic?
			*offset = curr_offset;
			return node;
			break;
		}
	}
};
// Note: dt_init.c must parse FDT header to get initial offset and size,
// then call parse_subtree(fdt_blob, &offset, struct_off + struct_size, NULL)
