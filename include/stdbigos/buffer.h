#ifndef _STDBIGOS_BUFFER_H
#define _STDBIGOS_BUFFER_H

#include <stdbigos/array_sizes.h>
#include <stdbigos/types.h>

typedef struct buffer_t {
	size_t size;
	const void* data __sized_by(size);
} buffer_t;

// Helpers to create buffers
[[nodiscard]]
static inline buffer_t make_buffer(const void* data, size_t size) {
	buffer_t buf = {.data = data, .size = size};
	return buf;
}

[[nodiscard]]
static inline bool buffer_is_valid(buffer_t buf) {
	return buf.data != nullptr;
}

[[nodiscard]]
static inline bool buffer_is_empty(buffer_t buf) {
	return !buffer_is_valid(buf) || buf.size == 0;
}

// Read big-endian 32-bit from buffer at given offset
[[nodiscard]]
bool buffer_read_u32_be(buffer_t buf, size_t offset, u32* out);

// Read big-endian 64-bit from buffer at given offset
[[nodiscard]]
bool buffer_read_u64_be(buffer_t buf, size_t offset, u64* out);

// Read little-endian 32-bit from buffer at given offset
[[nodiscard]]
bool buffer_read_u32_le(buffer_t buf, size_t offset, u32* out);

// Read little-endian 64-bit from buffer at given offset
[[nodiscard]]
bool buffer_read_u64_le(buffer_t buf, size_t offset, u64* out);

// Read a zero-terminated C-string from buf at offset and output it's length
[[nodiscard]]
bool buffer_read_cstring_len(buffer_t buf, size_t offset, const char** out_str, u64* len);

// Read a zero-terminated C-string from buf at offset
[[nodiscard]]
bool buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str);

// Sub buffer
[[nodiscard]]
buffer_t buffer_sub_buffer(buffer_t buf, size_t offset, size_t max_size);

#endif
