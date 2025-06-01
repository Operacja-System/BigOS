#ifndef _STDBIGOS_BUFFER_H
#define _STDBIGOS_BUFFER_H

#include <stdbigos/types.h>

// Error codes
#define BUFFER_OK            0
#define BUFFER_OUT_OF_BOUNDS -1

typedef struct buffer_t {
	const void* data;
	size_t size;
} buffer_t;

// Helper to create a buffer
static inline buffer_t make_buffer(const void* data, size_t size) {
	buffer_t buf = {.data = data, .size = size};
	return buf;
}

// Read big-endian 32-bit from buffer at given offset
int buffer_read_u32_be(buffer_t buf, size_t offset, u32* out);

// Read big-endian 64-bit from buffer at given offset
int buffer_read_u64_be(buffer_t buf, size_t offset, u64* out);

// Read little-endian 32-bit from buffer at given offset
int buffer_read_u32_le(buffer_t buf, size_t offset, u32* out);

// Read little-endian 64-bit from buffer at given offset
int buffer_read_u64_le(buffer_t buf, size_t offset, u64* out);

// Read a zero-terminated C-string from buf at offset
int buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str);

#endif
