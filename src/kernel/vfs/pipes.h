#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "vfs.h"

#define MAX_SERVERS 8

typedef struct QueryQueueNode_t {
	struct QueryQueueNode_t* next;
	u8 key;
} QueryQueueNode_t;

typedef struct QueryQueue_t {
	QueryQueueNode_t* head;
	QueryQueueNode_t* tail;
	u64 size;
} QueryQueue_t;

void query_queue_init(QueryQueue_t** queue);
void query_queue_push(QueryQueue_t* queue, u8 byte);
error_t query_queue_front(QueryQueue_t* queue, u8* out);
error_t query_queue_pop(QueryQueue_t* queue);

// Maybe something more here?
typedef struct Pipe_t {
	QueryQueue_t* query_queue;
} Pipe_t;

error_t pipe_create(pstring_t name, ServiceHandle_t* out);
error_t pipe_open(u64 idx, FtEntry_t** out);
error_t pipe_read(u64 idx, u32 bytes, u8* out);
error_t pipe_write(u64 idx, u32 bytes, u8* buff);
error_t pipe_remove(u64 idx);

#endif
