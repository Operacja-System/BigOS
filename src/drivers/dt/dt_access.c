#include <drivers/dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/sbi.h>
#include <stdbigos/string.h>

#include "dt_alloc.h"
#include "dt_node.h"
#include "dt_parser.h"

static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

// There has to be a better way to do this
static void gap(u8 size) {
	for (u8 i = 0; i < size; i++) sbi_puts("\t");
}

struct dt_node* dt_get_root(void) {
	return root_node;
}

static struct dt_node* find_child_by_name(struct dt_node* parent, const char* name) {
	for (struct dt_node* child = parent->first_child; child; child = child->next_sibling) {
		if (strcmp(child->name, name) == 0) {
			return child;
		}
	}
	return nullptr;
}

struct dt_node* dt_node_find(const char* path) {
	if (!path || path[0] != '/' || !root_node)
		return nullptr;

	if (strcmp(path, "/") == 0)
		return root_node;

	struct dt_node* current = root_node;
	const char* p = path + 1;

	while (*p) {
		const char* slash = p;
		while (*slash && *slash != '/') slash += 1;

		int len = slash - p;
		if (len == 0)
			break;

		char segment[64];
		if (len >= (int)sizeof(segment))
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

const char* dt_node_get_name(const struct dt_node* node) {
	if (!node)
		return nullptr;

	return node->name;
}

int dt_node_child_count(const struct dt_node* n) {
	int c = 0;
	for (struct dt_node* ch = n->first_child; ch; ch = ch->next_sibling) c++;
	return c;
}

struct dt_node* dt_node_get_child(const struct dt_node* n, int idx) {
	struct dt_node* ch = n ? n->first_child : NULL;
	for (; ch && idx > 0; ch = ch->next_sibling, idx--);
	return ch;
}

struct dt_prop* dt_find_prop(const struct dt_node* node, const char* name) {
	if (!node || !name)
		return nullptr;

	for (struct dt_prop* prop = node->props; prop; prop = prop->next_prop) {
		if (strcmp(prop->name, name) == 0) {
			return prop;
		}
	}

	return nullptr;
}

int dt_prop_read_u32(const struct dt_node* node, const char* name, u32* out) {
	struct dt_prop* prop = dt_find_prop(node, name);

	if (!prop || prop->data_length < 4 || !prop->value)
		return -1;

	u32 val = read_be32(prop->value);

	*out = val;

	return 0;
}

int dt_prop_read_u64(const struct dt_node* node, const char* name, u64* out) {
	struct dt_prop* prop = dt_find_prop(node, name);

	if (!prop || prop->data_length < 8 || !prop->value)
		return -1;

	u64 val = read_be64(prop->value);

	*out = val;

	return 0;
}

void dt_print_props(const struct dt_node* node, u8 depth) {
	struct dt_prop* next = node->props;
	while (next) {
		gap(depth);
		sbi_puts(next->name);
		sbi_puts("\n");
		next = next->next_prop;
	}
}

void dt_print_tree(const struct dt_node* node, u8 depth) {
	gap(depth);
	sbi_puts("NODE: ");
	sbi_puts(node->name);
	sbi_puts("\n");
	gap(depth);
	sbi_puts("PROPERTIES:\n");
	dt_print_props(node, depth + 1);
	sbi_puts("\n");
	struct dt_node* next = node->first_child;
	while (next) {
		dt_print_tree(next, depth + 1);
		next = next->next_sibling;
	}
}
