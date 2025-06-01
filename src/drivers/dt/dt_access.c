#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_node.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/string.h>

#include "dt_alloc.h"
#include "dt_parser.h"

static dt_node_t* find_child_by_name(dt_node_t* parent, const char* name) {
	for (dt_node_t* child = parent->first_child; child; child = child->next_sibling) {
		if (strcmp(child->name, name) == 0) {
			return child;
		}
	}
	return nullptr;
}

dt_node_t* dt_node_find(const char* path) {
	dt_node_t* root_node = dt_get_root();

	if (!path || path[0] != '/' || !root_node)
		return nullptr;

	if (strcmp(path, "/") == 0)
		return root_node;

	dt_node_t* current = root_node;
	const char* p = path + 1;

	while (*p) {
		const char* slash = p;
		while (*slash && *slash != '/') slash += 1;

		uintptr_t len = slash - p;
		if (len == 0)
			break;

		char segment[64];
		if (len >= (u32)sizeof(segment))
			return nullptr;

		memcpy(segment, p, len);
		segment[len] = '\0';

		current = find_child_by_name(current, segment);
		if (!current)
			return nullptr;

		p = (*slash == '/') ? slash + 1 : slash;
	}

	return current;
}

const char* dt_node_get_name(const dt_node_t* node) {
	if (!node)
		return nullptr;

	return node->name;
}

int dt_node_child_count(const dt_node_t* n) {
	int c = 0;
	for (dt_node_t* ch = n->first_child; ch; ch = ch->next_sibling) c++;
	return c;
}

dt_node_t* dt_get_next_child(const dt_node_t* node) {
	if (!node)
		return nullptr;

	// If there's no next sibling, it will automatically return a nullptr, as per documentation
	return node->next_sibling;
}

dt_prop_t* dt_find_prop(const dt_node_t* node, const char* name) {
	if (!node || !name)
		return nullptr;

	for (dt_prop_t* prop = node->props; prop; prop = prop->next_prop) {
		if (strcmp(prop->name, name) == 0) {
			return prop;
		}
	}

	return nullptr;
}

int dt_prop_read_u32(const dt_node_t* node, const char* name, u32* out) {
	dt_prop_t* prop = dt_find_prop(node, name);

	if (!prop || prop->data_length < 4 || !prop->value)
		return -1;

	u32 val = read_be32(prop->value);

	*out = val;

	return 0;
}

int dt_prop_read_u64(const dt_node_t* node, const char* name, u64* out) {
	dt_prop_t* prop = dt_find_prop(node, name);

	if (!prop || prop->data_length < 8 || !prop->value)
		return -1;

	u64 val = read_be64(prop->value);

	*out = val;

	return 0;
}

void dt_print_props(const dt_node_t* node, u8 depth) {
	dt_prop_t* next = node->props;
	while (next) {
		DEBUG_PUTGAP(depth);
		DEBUG_PRINTF("%s\n", next->name);
		next = next->next_prop;
	}
}

// Use only in debug preset
void dt_print_tree(const dt_node_t* node, u8 depth) {
	DEBUG_PUTGAP(depth);
	DEBUG_PRINTF("NODE: %s\n", node->name);
	DEBUG_PUTGAP(depth);

	DEBUG_PRINTF("PROPERTIES:\n");
	dt_print_props(node, depth + 1);
	DEBUG_PUTC('\n');
	dt_node_t* next = node->first_child;
	while (next) {
		dt_print_tree(next, depth + 1);
		next = next->next_sibling;
	}
}
