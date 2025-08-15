#ifndef _STDBIGOS_BUFFER_H
#define _STDBIGOS_BUFFER_H

#include <stdbigos/error.h>
#include <stdbigos/types.h>

// Error codes
typedef enum : u32 {
	BUFF_ERR_OK = 0,
	BUFF_ERR_OUT_OF_BOUNDS,
	BUFF_ERR_FETCH,
	BUFF_ERR_NOT_VALID,
} buffer_error_t;

typedef struct buffer_t {
	const void* data;
	size_t size;
	buffer_error_t error;
} buffer_t;

// Helpers to create buffers
static inline buffer_t make_buffer(const void* data, size_t size) {
	buffer_t buf = {.data = data, .size = size, .error = BUFF_ERR_OK};
	return buf;
}

static inline buffer_t make_buffer_err(const void* data, size_t size, buffer_error_t error) {
	buffer_t buf = {.data = data, .size = size, .error = error};
	return buf;
}

// Read big-endian 32-bit from buffer at given offset
error_t buffer_read_u32_be(buffer_t buf, size_t offset, u32* out);

// Read big-endian 64-bit from buffer at given offset
error_t buffer_read_u64_be(buffer_t buf, size_t offset, u64* out);

// Read little-endian 32-bit from buffer at given offset
error_t buffer_read_u32_le(buffer_t buf, size_t offset, u32* out);

// Read little-endian 64-bit from buffer at given offset
error_t buffer_read_u64_le(buffer_t buf, size_t offset, u64* out);

// Read a zero-terminated C-string from buf at offset
error_t buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str);

error_t buffer_read_u8(buffer_t buf, size_t offset, u8* out);
error_t buffer_read_i8(buffer_t buf, size_t offset, i8* out);
error_t buffer_read_u16(buffer_t buf, size_t offset, u16* out);
error_t buffer_read_i16(buffer_t buf, size_t offset, i16* out);
error_t buffer_read_u32(buffer_t buf, size_t offset, u32* out);
error_t buffer_read_i32(buffer_t buf, size_t offset, i32* out);
error_t buffer_read_u64(buffer_t buf, size_t offset, u64* out);
error_t buffer_read_i64(buffer_t buf, size_t offset, i64* out);

#endif
