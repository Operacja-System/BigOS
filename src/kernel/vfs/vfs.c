#include "vfs.h"

#include <debug/debug_stdio.h>
#include <debug/panic.h>
#include <stdbigos/assert.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>
#include <stdbigos/string.h>
#include <stddef.h>

#include "file_table.h"
#include "pipes.h"
#include "stdbigos/meta/err_x_t.h"
#include "stdbigos/types.h"

static void vfs_test_path_next() {
	pstring_t path1 = ERRX_UNWRAP(pstring_l2w("/foo/bar/baz/file.c"));
	VfsPath_t path = vfs_path_new(&path1);

	pstring_t edge = {.len = 0, .data = nullptr};
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
}

static void vfs_test_pipes() {
	FtEntry_t* example_file_entry1;
	FtEntry_t* example_file_entry2;

	char buff[100];

	example_file_entry1 = ft_add_entry();
	example_file_entry2 = ft_add_entry();
	kassert(pipe_create(&example_file_entry1->kernel_read_pipe, &example_file_entry2->kernel_write_pipe) == ERR_NONE);

	const char* message1 = "Hello from vfs! This message is brought to you via a Kernel Pipe.\n";
	const char* message2 = "Hello again, it seems that these pipes are not just for one time use!\n";
	const char* message3 = "How convenient!\n";
	const size_t message1_len = strlen(message1);
	const size_t message2_len = strlen(message2);
	const size_t message3_len = strlen(message3);

	// Write and read a mesage
	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, message1_len + 1, (u8*)message1)) ==
	        message1_len + 1);
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, message1_len + 1, (u8*)buff)) ==
	        message1_len + 1);
	DEBUG_PUTS(buff);

	// Write two messages at once and then read them with different sizes
	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, message2_len, (u8*)message2)) ==
	        message2_len);
	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, message3_len + 1, (u8*)message3)) ==
	        message3_len + 1);

	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, message3_len, (u8*)buff)) == message3_len);
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, 123456, (u8*)(buff + message3_len))) ==
	        (message2_len + 1));
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, 10, (u8*)buff)) == 0);

	DEBUG_PUTS(buff);

	ft_free_entry(example_file_entry1);
	ft_free_entry(example_file_entry2);
}

// Here just for debugging
void vfsmain() {
	DEBUG_PUTS("Hello VFS\n");

	DEBUG_PUTS("-> VFS showcase vfs_path_next:\n");
	vfs_test_path_next();
	DEBUG_PUTS("-> VFS showcase pipes:\n");
	vfs_test_pipes();

	DEBUG_PUTS("-> ALL vfs samples finished\n");
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
