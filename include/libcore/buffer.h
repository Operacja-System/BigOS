#ifndef LIBCORE_BUFFER
#define LIBCORE_BUFFER

#include <libcore/array_sizes.h>
#include <libcore/types.h>

/// @addtogroup libcore
/// @{
/// @addtogroup buffer
/// @{

typedef struct buffer_t {
	size_t size;
	const void* data __sized_by(size);
} buffer_t;

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

[[nodiscard]]
bool buffer_read_u32_be(buffer_t buf, size_t offset, u32* out);

[[nodiscard]]
bool buffer_read_u64_be(buffer_t buf, size_t offset, u64* out);

[[nodiscard]]
bool buffer_read_u32_le(buffer_t buf, size_t offset, u32* out);

[[nodiscard]]
bool buffer_read_u64_le(buffer_t buf, size_t offset, u64* out);

[[nodiscard]]
bool buffer_read_cstring_len(buffer_t buf, size_t offset, const char** out_str, u64* len);

[[nodiscard]]
bool buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str);

[[nodiscard]]
buffer_t buffer_sub_buffer(buffer_t buf, size_t offset, size_t max_size);

[[nodiscard]]
int buffer_memcmp(buffer_t lhs, buffer_t rhs);

[[nodiscard]]
bool buffer_equal(buffer_t lhs, buffer_t rhs);

/// @}
/// @}

#endif // !LIBCORE_BUFFER
