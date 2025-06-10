#include "vfs.h"

#include <debug/debug_stdio.h>
#include <stdbigos/assert.h>
#include <stdbigos/panic.h>
#include <stdbigos/pstring.h>
#include <stddef.h>

#include "vfs_alloc.h"

// Here just for debugging
void vfsmain() {
	DEBUG_PUTS("Hello VFS\n");
	pstring_t path1 = ERRX_UNWRAP(pstring_l2w("/foo/bar/baz/file.c"));
	VfsPath_t path = vfs_path_new(&path1);

	pstring_t edge;
	edge.data = vfs_malloc(1024);
	if (edge.data == nullptr) {
		PANIC("vfs_malloc failed");
	}
	edge.len = 1024;
	size_t i = 0;
	while (vfs_path_next(&path, &edge)) {
		if (edge.data == nullptr) {
			PANIC("vfs_path_next returned null edge data");
		}
		DEBUG_PRINTF("Path part %zu: ", i++);
		for (size_t i = 0; i < edge.len; i++) {
			DEBUG_PUTC(edge.data[i]);
		}
		DEBUG_PUTC('\n');
	}
	kassert("foo" == nullptr); // Just for testing
	PANIC("This should not be reached");
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
