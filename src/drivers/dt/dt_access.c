#include <drivers/dt/dt.h>
#include <drivers/dt/dt_node.h>
#include <drivers/dt/dt_parser.h>
#include <drivers/dt/dt_props.h>
#include <stdbigos/string.h>

struct dt_node* dt_get_root(void) {
	return root_node;
}

static struct dt_node* find_child_by_name(struct dt_node* parent, const char* name) {
	for (struct dt_node* child = parent->first_child; child; child = child->next_sibling) {
		if (strcmp(child->name, name) == 0) {
			return child;
		}
	}
	return ((void*)0);
}

struct dt_node* dt_find_node(const char* path) {
	if (!path || path[0] != '/' || !root_node)
		return ((void*)0);

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
			return ((void*)0);

		memcpy(segment, p, len);
		segment[len] = '\0';

		current = find_child_by_name(current, segment);
		if (!current)
			return ((void*)0);

		p = (*slash == '/') ? slash + 1 : slash;
	}

	return current;
}

struct dt_prop* dt_find_prop(const struct dt_node* node, const char* name) {
	if (!node || !name)
		return ((void*)0);

	for (struct dt_prop* prop = node->props; prop; prop = prop->next_prop) {
		if (strcmp(prop->name, name) == 0) {
			return prop;
		}
	}

	return ((void*)0);
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

	const u8* val = (const u8*)prop->value;

	u32 high = read_be32(val);
	u32 low = read_be32(val + 4);

	*out = ((u64)high << 32) | (u64)low;
	return 0;
}
