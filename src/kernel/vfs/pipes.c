#include "pipes.h"

#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "file_table.h"
#include "vfs.h"
#include "vfs_alloc.h"

// Currently pipe files are stored in an array. Finding them by name takes O(MAX_SERVERS) operations.
// Since this number will most likely be small, this should be sufficient, but TODO: perhaps we want something better
// here anyway
Pipe_t pipes[MAX_SERVERS];
bool is_pipe_slot_in_usage[MAX_SERVERS];

void query_queue_init(QueryQueue_t** queue) {
	*queue = (QueryQueue_t*)vfs_malloc(sizeof(QueryQueue_t));
	(*queue)->head = (*queue)->tail = nullptr;
	(*queue)->size = 0;
}

void query_queue_push(QueryQueue_t* queue, u8 byte) {
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

error_t query_queue_front(QueryQueue_t* queue, u8* out) {
	if (!queue->size)
		return ERR_QUEUE_EMPTY;
	*out = queue->head->key;
	return ERR_NONE;
}

error_t query_queue_pop(QueryQueue_t* queue) {
	if (!queue->size)
		return ERR_QUEUE_EMPTY;
	QueryQueueNode_t* old_head = queue->head;
	queue->head = old_head->next;
	vfs_free(old_head);
	queue->size--;
	return ERR_NONE;
}

error_t pipe_create(pstring_t name, ServiceHandle_t* out) {
	i64 free_idx = -1;
	for (i64 i = 0; i < MAX_SERVERS; i++) {
		if (!is_pipe_slot_in_usage[i]) {
			free_idx = i;
			break;
		}
	}
	if (free_idx == -1) {
		return ERR_SERVER_FS_FULL;
	}
	is_pipe_slot_in_usage[free_idx] = true;
	query_queue_init(&pipes[free_idx].query_queue);

	*out = (Service_t*)vfs_malloc(sizeof(Service_t));
	(*out)->pipe_id = free_idx;
	(*out)->service_name = name;
	return ERR_NONE;
}

error_t pipe_open(u64 idx, FtEntry_t** out) {
	if (!is_pipe_slot_in_usage[idx]) {
		return ERR_FILE_NOT_FOUND;
	}
	*out = ft_add_entry(PIPE_HANDLE, idx, 0); // TODO: Attributes
	return ERR_NONE;
}

error_t pipe_read(u64 idx, u32 bytes, u8* out) {
	if (!is_pipe_slot_in_usage[idx])
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
	if (!is_pipe_slot_in_usage[idx])
		return ERR_BROKEN_FILE_DESCRIPTOR;
	Pipe_t* file = &(pipes[idx]);
	for (u32 i = 0; i < bytes; i++) {
		query_queue_push(file->query_queue, buff[i]);
	}
	return ERR_NONE;
}

error_t pipe_remove(u64 idx) {
	if (!is_pipe_slot_in_usage[idx]) {
		return ERR_FILE_NOT_FOUND;
	}
	is_pipe_slot_in_usage[idx] = false;
	return ERR_NONE;
}
