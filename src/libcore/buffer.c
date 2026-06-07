#include <libcore/bitutils.h>
#include <libcore/buffer.h>
#include <libcore/math.h>
#include <libcore/string.h>
#include <libcore/types.h>

bool buffer_read_u32_be(buffer_t buf, size_t offset, u32* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = read_be32((const u8*)buf.data + offset);
	return ok;
}

bool buffer_read_u32_le(buffer_t buf, size_t offset, u32* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = read_le32((const u8*)buf.data + offset);
	return ok;
}
bool buffer_read_u64_be(buffer_t buf, size_t offset, u64* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = read_be64((const u8*)buf.data + offset);
	return ok;
}
bool buffer_read_u64_le(buffer_t buf, size_t offset, u64* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = read_le64((const u8*)buf.data + offset);
	return ok;
}

bool buffer_read_cstring_len(buffer_t buf, size_t offset, const char** out_str, u64* len) {
	if (!buffer_is_valid(buf) || !out_str || offset >= buf.size)
		return false;

	const char* beg = (const char*)buf.data + offset;
	const void* end = memchr(beg, '\0', buf.size - offset);

	if (!end)
		return false;

	// found null
	*out_str = beg;

	if (len)
		*len = (u64)((const char*)end - beg);

	return true;
}

bool buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str) {
	return buffer_read_cstring_len(buf, offset, out_str, nullptr);
}

buffer_t buffer_sub_buffer(buffer_t buf, size_t offset, size_t max_size) {
	if (!buffer_is_valid(buf) || buf.size < offset)
		return make_buffer(nullptr, 0);

	size_t rest = buf.size - offset;
	return make_buffer((const u8*)buf.data + offset, MIN(rest, max_size));
}

int buffer_memcmp(buffer_t lhs, buffer_t rhs) {
	size_t sz = MIN(lhs.size, rhs.size);
	int ret = memcmp(lhs.data, rhs.data, sz);
	if (ret)
		return ret;
	if (lhs.size < rhs.size)
		return -1;
	if (lhs.size > rhs.size)
		return 1;
	return 0;
}

bool buffer_equal(buffer_t lhs, buffer_t rhs) {
	return lhs.size == rhs.size && memcmp(lhs.data, rhs.data, lhs.size) == 0;
}
