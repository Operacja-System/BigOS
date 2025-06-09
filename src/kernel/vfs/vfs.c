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
	if (!path || pstring_len_silenterr(&path->path) == 0) {
		return false;
	}

	const pstring_t vpath = path->path;
	pstring_t split = ERRX_UNWRAP(pstring_strchr(&vpath, '/'));
	if (split.len == 0) {
		path->path.data = nullptr;
		path->path.len = 0;
		*edge = vpath;
	}

	size_t split_pos = vpath.len - split.len;
	*edge = ERRX_UNWRAP(pstring_slice_view(&vpath, 0, split_pos));
	path->path = ERRX_UNWRAP(pstring_slice_view(&vpath, split_pos + 1, vpath.len));
	return true;
}
