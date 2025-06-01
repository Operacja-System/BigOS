#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_node.h>
#include <drivers/dt/dt_props.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/string.h>

#include "dt_alloc.h"
#include "dt_parser.h"

static dt_node_t* find_child_by_name_s(dt_node_t* parent, const char* name) {
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

		current = find_child_by_name_s(current, segment);
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

buffer_t dt_prop_get_buffer(const dt_node_t* node, const char* name) {
	buffer_t buffer = make_buffer_err(nullptr, 0, BUF_ERR_FETCH);

	dt_prop_t* prop = dt_find_prop(node, name);
	if (!prop || !prop->value)
		return buffer;

	buffer.data = prop->value;
	buffer.size = prop->data_length;
	buffer.error = BUF_ERR_OK;

	return buffer;
}

#ifndef NDEBUG

// Use only in debug preset
void dt_print_props(const dt_node_t* node, [[maybe_unused]] u8 depth) {
	dt_prop_t* next = node->props;
	while (next) {
		dputgap(depth);
		dprintf("%s\n", next->name);
		next = next->next_prop;
	}
}

// Use only in debug preset
void dt_print_tree(const dt_node_t* node, [[maybe_unused]] u8 depth) {
	dputgap(depth);
	dprintf("NODE: %s\n", node->name);
	dputgap(depth);

	dprintf("PROPERTIES:\n");
	dt_print_props(node, depth + 1);
	dputc('\n');
	dt_node_t* next = node->first_child;
	while (next) {
		dt_print_tree(next, depth + 1);
		next = next->next_sibling;
	}
}
#else
void dt_print_props(const dt_node_t* node, [[maybe_unused]] u8 depth);
void dt_print_tree(const dt_node_t* node, [[maybe_unused]] u8 depth);
#endif
