#include "vfs.h"

#include <debug/debug_stdio.h>
#include <stdbigos/pstring.h>
#include <stddef.h>

// Here just for debugging
void vfsmain() {
	DEBUG_PUTS("Hello VFS\n");
}

VfsPath_t vfs_path_new(const VfsPathBuff_t* path_buff) {
	return (VfsPath_t){.path = *path_buff};
}

bool vfs_path_next(VfsPath_t* path, pstring_t* edge) {
	if (!path || pstring_len(&path->path) == 0) {
		return false;
	}

	const pstring_t vpath = path->path;
	char* split = pstring_strchr(&vpath, '/');
	if (split == nullptr) {
		path->path.data = nullptr;
		path->path.len = 0;
		*edge = vpath;
	}

	u32 split_pos = vpath.data - split;
	*edge = pstring_slice_view(&vpath, 0, split_pos);
	path->path = pstring_slice_view(&vpath, split_pos + 1, vpath.len);
	return true;
}
