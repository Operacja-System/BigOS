#include "mount_tree.h"

#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "vfs.h"
#include "vfs_alloc.h"

error_t mt_init(MtNode_t** res) {
	*res = vfs_malloc(sizeof(MtNode_t));
	if (!*res) {
		return ERR_MALLOC_FAILED;
	}

	// TODO: Fill in service information for the mount tree root
	(*res)->edge_list = nullptr;

	return ERR_NONE;
}

// Currently this only frees the subtree.
// In the future we will most likely need to do more 'stuff' here
void mt_free(MtNode_t* node) {
	if (!node) {
		return;
	}

	MtEdgeList_t* edge_list = node->edge_list;
	while (edge_list) {
		vfs_free(edge_list->edge.label.data);
		mt_free(edge_list->edge.to);

		MtEdgeList_t* curr = edge_list;
		edge_list = edge_list->next;
		vfs_free(curr);
	}
}

bool mt_step(const MtNode_t* node, const pstring_t* label, MtNode_t** out) {
	if (!node || !label)
		return false;

	for (MtEdgeList_t* curr = node->edge_list; curr; curr = curr->next) {
		if (ERRX_UNWRAP(pstring_strcmp(&curr->edge.label, label)) == 0) {
			*out = curr->edge.to;
			return true;
		}
	}

	return false;
}

bool mt_walk(const MtNode_t* node, VfsPath_t* path, MtNode_t** out) {
	VfsPath_t curr_path = *path;
	pstring_t curr;
	while (vfs_path_next(&curr_path, &curr)) {
		if (!mt_step(node, &curr, out)) {
			return false;
		}

		*path = curr_path;
	}

	return true;
}

// TODO: Error handling? In case of an error we probably need to clean the partially created subtree.
//       But nothing really happens if we don't so its only 'TODO'
/// Adds nodes so the `new_node` is accessible by walking `path` from `node`
static error_t mt_add_nodes(MtNode_t* node, VfsPath_t path, MtNode_t** new_node) {
	bool res = mt_walk(node, &path, new_node);

	// Path already walkable -> nothing to do
	if (res) {
		return ERR_NONE;
	}

	node = *new_node;
	pstring_t curr_label;
	while (vfs_path_next(&path, &curr_label)) {
		*new_node = vfs_malloc(sizeof(MtNode_t));
		if (!*new_node) {
			return ERR_MALLOC_FAILED;
		}
		(*new_node)->edge_list = nullptr;

		// Copy the current label to own the memory behind it
		pstring_t curr_label_copy = (pstring_t){.len = curr_label.len, .data = vfs_malloc(curr_label.len)};
		if (!curr_label_copy.data) {
			return ERR_MALLOC_FAILED;
		}
		ERRX_UNWRAP(pstring_memcpy(&curr_label_copy, &curr_label));

		MtEdgeList_t* new_list_element = vfs_malloc(sizeof(MtEdgeList_t));
		if (!*new_node) {
			return ERR_MALLOC_FAILED;
		}
		new_list_element->edge = (MtEdge_t){
		    .label = curr_label_copy,
		    .to = *new_node,
		};

		// Add new list element to the front
		new_list_element->next = node->edge_list;
		node->edge_list = new_list_element;

		node = *new_node;
	}

	return ERR_NONE;
}

error_t mt_add_mountpoint(MtNode_t* root, VfsPath_t path, ServiceHandle_t service, MtNode_t** out) {
	error_t res = mt_add_nodes(root, path, out);
	if (res != ERR_NONE) {
		return res;
	}

	if ((*out)->service != nullptr) {
		return ERR_MT_MOUNTPOINT_EXSITS;
	}

	(*out)->service = service;
	return ERR_NONE;
}
