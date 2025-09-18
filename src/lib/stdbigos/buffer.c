#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

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

bool buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str) {
	if (!buffer_is_valid(buf) || !out_str || offset >= buf.size)
		return false;

	const char* beg = (const char*)buf.data + offset;
	const void* end = memchr(beg, '\0', buf.size - offset);

	if (!end)
		return false;

	// found null
	*out_str = beg;
	return true;
}

buffer_t buffer_sub_buffer(buffer_t buf, size_t offset, size_t max_size) {
	if (!buffer_is_valid(buf) || buf.size < offset)
		return make_buffer(nullptr, 0);
	size_t rest = buf.size - offset;
	return make_buffer((const u8*)buf.data + offset, MIN(rest, max_size));
}

bool buffer_read_u8(buffer_t buf, size_t offset, u8* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const u8*)(buf.data + offset);
	return ok;
}

bool buffer_read_i8(buffer_t buf, size_t offset, i8* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const i8*)(buf.data + offset);
	return ok;
}

bool buffer_read_u16(buffer_t buf, size_t offset, u16* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const u16*)(buf.data + offset);
	return ok;
}

bool buffer_read_i16(buffer_t buf, size_t offset, i16* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const i16*)(buf.data + offset);
	return ok;
}

bool buffer_read_u32(buffer_t buf, size_t offset, u32* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const u32*)(buf.data + offset);
	return ok;
}

bool buffer_read_i32(buffer_t buf, size_t offset, i32* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const i32*)(buf.data + offset);
	return ok;
}

bool buffer_read_u64(buffer_t buf, size_t offset, u64* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const u64*)(buf.data + offset);
	return ok;
}

bool buffer_read_i64(buffer_t buf, size_t offset, i64* out) {
	bool ok = buffer_is_valid(buf) && offset + sizeof(*out) <= buf.size;
	if (ok)
		*out = *(const i64*)(buf.data + offset);
	return ok;
}
