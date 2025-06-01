#include <stdbigos/buffer.h>

int buffer_read_u32_be(buffer_t buf, size_t offset, u32* out) {
	if (!out || offset + 4 > buf.size)
		return BUF_ERR_OUT_OF_BOUNDS;
	const u8* b = (const u8*)buf.data + offset;
	*out = ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | (u32)b[3];
	return BUF_ERR_OK;
}

int buffer_read_u64_be(buffer_t buf, size_t offset, u64* out) {
	if (!out || offset + 8 > buf.size)
		return BUF_ERR_OUT_OF_BOUNDS;
	const u8* b = (const u8*)buf.data + offset;
	*out = ((u64)b[0] << 56) | ((u64)b[1] << 48) | ((u64)b[2] << 40) | ((u64)b[3] << 32) | ((u64)b[4] << 24) |
	       ((u64)b[5] << 16) | ((u64)b[6] << 8) | (u64)b[7];
	return BUF_ERR_OK;
}

int buffer_read_u32_le(buffer_t buf, size_t offset, u32* out) {
	if (!out || offset + 4 > buf.size)
		return BUF_ERR_OUT_OF_BOUNDS;
	const u8* b = (const u8*)buf.data + offset;
	*out = ((u32)b[3] << 24) | ((u32)b[2] << 16) | ((u32)b[1] << 8) | (u32)b[0];
	return BUF_ERR_OK;
}

int buffer_read_u64_le(buffer_t buf, size_t offset, u64* out) {
	if (!out || offset + 8 > buf.size)
		return BUF_ERR_OUT_OF_BOUNDS;
	const u8* b = (const u8*)buf.data + offset;
	*out = ((u64)b[7] << 56) | ((u64)b[6] << 48) | ((u64)b[5] << 40) | ((u64)b[4] << 32) | ((u64)b[3] << 24) |
	       ((u64)b[2] << 16) | ((u64)b[1] << 8) | (u64)b[0];
	return BUF_ERR_OK;
}

int buffer_read_cstring(buffer_t buf, size_t offset, const char** out_str) {
	if (!buf.data || !out_str || offset >= buf.size)
		return BUF_ERR_OUT_OF_BOUNDS;

	const char* base = (const char*)buf.data;
	size_t i = offset;
	// scan until '\0' or end‐of‐buffer
	while (i < buf.size && base[i] != '\0') {
		i++;
	}
	if (i >= buf.size)
		return BUF_ERR_OUT_OF_BOUNDS; // no terminator inside buffer

	*out_str = base + offset;
	return BUF_ERR_OK;
}
