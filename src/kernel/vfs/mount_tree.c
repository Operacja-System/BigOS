#include "mount_tree.h"

#include <debug/debug_stdio.h>
#include <stdbigos/string.h>

#include "vfs.h"

MT_NODE nodes[MT_MAX_NODES]; // temporary solution, while allocator is not available
int free_node_idx;

MT_NODE* mt_init()           // placeholder for now
{
	free_node_idx = 0;
	return nodes;
}

bool mt_find_edge_by_label(MT_NODE* node, char* label, MT_NODE** out) {
	if (!node)
		return false;

	for (int i = 0; i < node->edges_size; i++) {
		if (strcmp(node->edges[i].label, label) == 0) {
			*out = node->edges[i].to;
			return true;
		}
	}
	return false;
}

MT_ADD_NODE_STATUS mt_add_node(MT_NODE* node, char* label, SERVICE_HANDLE service) {
	MT_ADD_NODE_STATUS ret;
	MT_NODE* dummy = nullptr;
	if (mt_find_edge_by_label(node, label, &dummy)) {
		ret.err = ERROR_MT_TRIED_TO_ADD_EDGE_WHICH_EXISTS;
		return ret;
	}
	MT_NODE* new_node = nodes + free_node_idx++;
	MT_EDGE new_edge;
	new_edge.label = label;
	new_edge.to = new_node;
	node->edges[node->edges_size++] = new_edge;
	new_node->edges_size = 0;
	new_node->service = service;
	ret.err = 0;
	ret.res = new_node;
	return ret;
}

MT_ADD_MOUNTPOINT_STATUS mt_add_mountpoint(MT_NODE* root, VFS_PATH* path, SERVICE_HANDLE service) {
	MT_ADD_MOUNTPOINT_STATUS ret;
	MT_NODE* node = root;
	int idx = 0;
	MT_NODE* next_node = nullptr;
	while (idx < path->size && mt_find_edge_by_label(node, path->path[idx], &next_node)) {
		node = next_node;
		idx++;
	}
	for (int i = idx; i < path->size; i++) {
		MT_ADD_NODE_STATUS status;
		status = mt_add_node(node, path->path[i], nullptr);
		if (status.err) {
			ret.err = ERROR_MT_TRIED_TO_ADD_EDGE_WHICH_EXISTS;
			return ret;
		}
		node = status.res;
	}
	if (node->service) {
		ret.err = ERROR_MT_MOUNTPOINT_EXSITS;
		return ret;
	}
	node->service = service;
	ret.err = 0;
	ret.res = node;
	return ret;
}
