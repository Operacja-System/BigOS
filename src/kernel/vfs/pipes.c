#include "pipes.h"

#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "file_table.h"
#include "vfs.h"
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

error_t pipe_create(KernelPipe_t* out) {
	i64 free_idx = -1;
	for (i64 i = 0; i < MAX_SERVERS; i++) {
		if (!pipes[i].is_used) {
			free_idx = i;
			break;
		}
	}
	if (free_idx == -1) {
		return ERR_SERVER_FS_FULL;
	}
	pipes[free_idx].is_used = true;
	query_queue_init(&pipes[free_idx].query_queue);

	*out = (KernelPipe_t){
	    .pipe_id = free_idx,
	    .attributes = 0, // TODO: Create KernelPipe_t attributes
	};
	out->pipe_id = free_idx;
	return ERR_NONE;
}

error_t pipe_read(u64 idx, u32 bytes, u8* out) {
	if (!pipes[idx].is_used)
		return ERR_BROKEN_FILE_DESCRIPTOR;
	Pipe_t* file = &(pipes[idx]);
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

error_t pipe_write(u64 idx, u32 bytes, u8* buff) {
	if (!pipes[idx].is_used)
		return ERR_BROKEN_FILE_DESCRIPTOR;
	Pipe_t* file = &(pipes[idx]);
	for (u32 i = 0; i < bytes; i++) {
		query_queue_push(file->query_queue, buff[i]);
	}
	return ERR_NONE;
}

error_t pipe_remove(u64 idx) {
	if (!pipes[idx].is_used) {
		return ERR_FILE_NOT_FOUND;
	}
	pipes[idx].is_used = false;
	return ERR_NONE;
}
