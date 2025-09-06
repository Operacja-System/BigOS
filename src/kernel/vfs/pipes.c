#include "pipes.h"

#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "vfs_alloc.h"

// TODO: Implement this as a circular buffer.
typedef struct QueryQueueNode_t {
	struct QueryQueueNode_t* next;
	u8 key;
} QueryQueueNode_t;

typedef struct QueryQueue_t {
	QueryQueueNode_t* head;
	QueryQueueNode_t* tail;
	u64 size;
} QueryQueue_t;

// Currently pipe files are stored in an array. Finding them by name takes O(MAX_SERVERS) operations.
// Since this number will most likely be small, this should be sufficient, but:
// TODO: perhaps we want something better here.
typedef struct Pipe_t {
	QueryQueue_t* query_queue;
	u16 readers;
	u16 writers;
	bool is_used;
} Pipe_t;
static Pipe_t pipes[MAX_SERVERS];

static void query_queue_init(QueryQueue_t** queue) {
	*queue = (QueryQueue_t*)vfs_malloc(sizeof(QueryQueue_t));
	(*queue)->head = (*queue)->tail = nullptr;
	(*queue)->size = 0;
}

static void query_queue_push(QueryQueue_t* queue, u8 byte) {
	QueryQueueNode_t* new_node = (QueryQueueNode_t*)vfs_malloc(sizeof(QueryQueueNode_t));
	new_node->key = byte;
	if (queue->size == 0) {
		queue->head = queue->tail = new_node;
	} else {
		queue->tail->next = new_node;
		queue->tail = new_node;
	}
	queue->size++;
}

static error_t query_queue_front(QueryQueue_t* queue, u8* out) {
	if (!queue->size)
		return ERR_QUEUE_EMPTY;
	*out = queue->head->key;
	return ERR_NONE;
}

static error_t query_queue_pop(QueryQueue_t* queue) {
	if (!queue->size)
		return ERR_QUEUE_EMPTY;
	QueryQueueNode_t* old_head = queue->head;
	queue->head = old_head->next;
	vfs_free(old_head);
	queue->size--;
	return ERR_NONE;
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
	query_queue_init(&pipes[free_idx].query_queue);

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

error_t pipe_read(KernelReadPipe_t* read_end, u32 bytes, u8* out) {
	// TODO: Check if there are writers left when no data is present
	Pipe_t* file = &(pipes[read_end->pipe_id]);
	for (u32 i = 0; i < bytes; i++) {
		u8 byte;
		if (query_queue_front(file->query_queue, &byte) == ERR_QUEUE_EMPTY) {
			return ERR_END_OF_FILE; // TODO: Wait for more input? (we need scheduler first)
		}

		query_queue_pop(file->query_queue);
		out[i] = byte;
	}

	return ERR_NONE;
}

error_t pipe_write(KernelWritePipe_t* write_end, u32 bytes, u8* buff) {
	// TODO: Check if there are readers left, how would we notify the user if there aren't any?
	Pipe_t* file = &(pipes[write_end->pipe_id]);
	for (u32 i = 0; i < bytes; i++) {
		query_queue_push(file->query_queue, buff[i]);
	}

	return ERR_NONE;
}

static void close_pipe_if_unused(Pipe_t* pipe) {
	if (pipe->writers == 0 && pipe->readers == 0) {
		pipe->is_used = false;
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
