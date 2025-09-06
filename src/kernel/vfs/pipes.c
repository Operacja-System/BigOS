#include "pipes.h"

#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "stdbigos/types.h"
#include "vfs_alloc.h"

typedef struct CircularBuffer_t {
	u32 start;
	u32 end;
	u32 size;
	bool is_filled;
	u8* buffer;
} CircularBuffer_t;

// Currently pipe files are stored in an array. Finding them by name takes O(MAX_SERVERS) operations.
// Since this number will most likely be small, this should be sufficient, but:
// TODO: Add a locking mechanism for concurrent read/writes
typedef struct Pipe_t {
	u16 readers;
	u16 writers;
	bool is_used;
	CircularBuffer_t buffer;
} Pipe_t;
static Pipe_t pipes[MAX_SERVERS];

static error_t circular_buffer_init(CircularBuffer_t* circular_buffer, u32 size) {
	*circular_buffer = (CircularBuffer_t){
	    .start = 0,
	    .end = 0,
	    .size = size,
	    .is_filled = false,
	    .buffer = vfs_malloc(size),
	};
	if (circular_buffer->buffer == nullptr) {
		return ERR_MALLOC_FAILED;
	}
	return ERR_NONE;
}

static u32 circular_buffer_space_left(const CircularBuffer_t* circular_buffer) {
	u32 free_space = circular_buffer->start < circular_buffer->end
	                     ? circular_buffer->start + circular_buffer->size - circular_buffer->end
	                     : circular_buffer->end - circular_buffer->start;
	if (free_space == 0 && !circular_buffer->is_filled) {
		free_space = circular_buffer->size;
	}
	return free_space;
}

static u32 circular_buffer_data_count(const CircularBuffer_t* circular_buffer) {
	u32 writable_bytes = circular_buffer->start <= circular_buffer->end
	                         ? circular_buffer->end - circular_buffer->start
	                         : circular_buffer->size - circular_buffer->start + circular_buffer->end;
	if (writable_bytes == 0 && circular_buffer->is_filled) {
		writable_bytes = circular_buffer->size;
	}
	return writable_bytes;
}

static size_t circular_buffer_read(CircularBuffer_t* circular_buffer, u32 count, u8* out) {
	const u32 data_count = circular_buffer_data_count(circular_buffer);
	const u32 bytes_to_read = count <= data_count ? count : data_count;

	u32 out_position = 0;
	while (out_position < bytes_to_read && circular_buffer->start < circular_buffer->size) {
		out[out_position++] = circular_buffer->buffer[circular_buffer->start++];
	}

	if (circular_buffer->start == circular_buffer->size) {
		circular_buffer->start = 0;
		while (out_position < bytes_to_read) {
			out[out_position++] = circular_buffer->buffer[circular_buffer->start++];
		}
	}

	circular_buffer->is_filled &= !(count > 0 && circular_buffer->start == circular_buffer->end);
	return out_position;
}

static size_t circular_buffer_write(CircularBuffer_t* circular_buffer, u32 count, const u8* data) {
	const u32 space_left = circular_buffer_space_left(circular_buffer);
	const u32 bytes_to_write = count <= space_left ? count : space_left;

	u32 data_position = 0;
	while (data_position < bytes_to_write && circular_buffer->end < circular_buffer->size) {
		circular_buffer->buffer[circular_buffer->end++] = data[data_position++];
	}

	if (circular_buffer->end == circular_buffer->size) {
		circular_buffer->end = 0;
		while (data_position < bytes_to_write) {
			circular_buffer->buffer[circular_buffer->end++] = data[data_position++];
		}
	}

	circular_buffer->is_filled |= (count > 0);
	return data_position;
}

static void circular_buffer_free(CircularBuffer_t* buffer) {
	vfs_free(buffer->buffer);
}

error_t pipe_create(KernelReadPipe_t* read_end, KernelWritePipe_t* write_end) {
	i64 free_idx = -1;
	for (i64 i = 0; i < MAX_SERVERS; i++) {
		if (!pipes[i].is_used) {
			free_idx = i;
			break;
		}
	}

	if (free_idx == -1) {
		return ERR_PIPE_FS_FULL;
	}

	Pipe_t* pipe = &(pipes[free_idx]);
	pipe->is_used = true;
	pipe->readers = pipe->writers = 1;
	circular_buffer_init(&pipe->buffer, PIPE_CAPACITY);

	*read_end = (KernelReadPipe_t){
	    .pipe_id = free_idx,
	    .attributes = 0, // TODO: Create Kernel Pipe attributes
	};
	*write_end = (KernelWritePipe_t){
	    .pipe_id = free_idx,
	    .attributes = 0, // TODO: Create Kernel Pipe attributes
	};

	return ERR_NONE;
}

size_or_err_t pipe_read(KernelReadPipe_t* read_end, u32 bytes, u8* out) {
	// TODO: Check if there are writers left when no data is present
	// TODO: Guarantee exclusive access
	// TODO: Fail if there are no data or writers
	Pipe_t* pipe = &(pipes[read_end->pipe_id]);
	return (size_or_err_t){.val = circular_buffer_read(&pipe->buffer, bytes, out)};
}

size_or_err_t pipe_write(KernelWritePipe_t* write_end, u32 bytes, u8* buff) {
	// TODO: Check if there are readers left, how would we notify the user if there aren't any?
	// TODO: Guarantee exclusive access
	// TODO: Fail if there are no readers
	Pipe_t* pipe = &(pipes[write_end->pipe_id]);
	return (size_or_err_t){.val = circular_buffer_write(&pipe->buffer, bytes, buff)};
}

static void close_pipe_if_unused(Pipe_t* pipe) {
	if (pipe->writers == 0 && pipe->readers == 0) {
		pipe->is_used = false;
		circular_buffer_free(&pipe->buffer);
	}
}

void pipe_close_read(KernelReadPipe_t* write_end) {
	Pipe_t* pipe = &(pipes[write_end->pipe_id]);
	pipe->readers--;
	close_pipe_if_unused(pipe);
}

void pipe_close_write(KernelWritePipe_t* read_end) {
	Pipe_t* pipe = &(pipes[read_end->pipe_id]);
	pipe->writers--;
	close_pipe_if_unused(pipe);
}
