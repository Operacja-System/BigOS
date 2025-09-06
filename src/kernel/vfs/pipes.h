#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#define MAX_SERVERS   8
#define PIPE_CAPACITY 4096
// TODO: Assert capacity is a power of two

// Read end of Kernel pipe, not managed by fs_server_protocol
typedef struct KernelReadPipe_t {
	u64 pipe_id;
	u64 attributes;
} KernelReadPipe_t;

// Write end of Kernel pipe, not managed by fs_server_protocol
typedef struct KernelWritePipe_t {
	u64 pipe_id;
	u64 attributes;
} KernelWritePipe_t;

typedef struct KernelPipe_t {
	KernelReadPipe_t read_end;
	KernelWritePipe_t write_end;
} KernelPipe_t;

[[nodiscard]] error_t pipe_create(KernelReadPipe_t* read_end, KernelWritePipe_t* write_end);
[[nodiscard]] size_or_err_t pipe_read(KernelReadPipe_t* read_end, u32 bytes, u8* out);
[[nodiscard]] size_or_err_t pipe_write(KernelWritePipe_t* write_end, u32 bytes, u8* buff);
void pipe_close_read(KernelReadPipe_t* write_end);
void pipe_close_write(KernelWritePipe_t* read_end);

#endif
