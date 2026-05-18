#include "hal/include/initramfs.h"

#include "dt/dt.h"
#include "hal_internal.h"
#include "stdbigos/buffer.h"
#include "stdbigos/string.h"

/*
 * initramfs layout (big endian)
 *
 * u32 magic
 * u32 version
 * u16 - file_count
 *
 *	u32 - offset (from the start of initramfs)
 *	u32 - size
 *	u8 - namesize
 *	char[] - name
 *	...
 *
 * data blob
 * */

static error_t validate(buffer_t initramfs_buff) {
	const u32 INITRAMFS_MAGIC = 0xB16B00B5;
	u32 magic = 0;
	u32 version = 0;
	if (!buffer_read_u32_be(initramfs_buff, 0, &magic))
		return ERR_INVALID_MEMORY_REGION;
	if (magic != INITRAMFS_MAGIC)
		return ERR_INVALID_MEMORY_REGION;
	if (!buffer_read_u32_be(initramfs_buff, 4, &version))
		return ERR_INVALID_MEMORY_REGION;
	// TODO: Handle version
	return ERR_NONE;
}

static error_t find_initramfs_buffer(buffer_t* buffOUT) {
	void* dtbptr = nullptr;
	if (ihal_get_dtb(&dtbptr) != ERR_NONE)
		return ERR_NOT_INITIALIZED;
	fdt_t fdt;
	(void)dt_init(dtbptr, &fdt); // This error is checkd on hal init
	dt_node_t initramfs_node;
	error_t err = dt_get_node_by_path(&fdt, "initramfs", &initramfs_node);
	if (err)
		return ERR_DT_NODE_NOT_FOUND;
	dt_prop_t initramfs_reg;
	err = dt_get_prop_by_name(&fdt, initramfs_node, "reg", &initramfs_reg);
	if (err)
		return ERR_DT_PROP_NOT_FOUND;
	return dt_get_prop_buffer(&fdt, initramfs_reg, buffOUT);
}

typedef struct {
	u32 offset;
	u32 size;
} file_header_t;

// TODO: Change filename to string_view
static error_t find_file_by_name(const char* filename, buffer_t initramfs_buff, file_header_t* headerOUT) {
	u64 cursor = 8;
	u16 filecount = 0;
	if (!buffer_read_u16_be(initramfs_buff, cursor, &filecount))
		return ERR_INVALID_MEMORY_REGION;
	cursor += sizeof(u16);
	for (u16 i = 0; i < filecount; ++i) {
		if (!buffer_read_u32_be(initramfs_buff, cursor, &headerOUT->offset))
			return ERR_INVALID_MEMORY_REGION;
		cursor += sizeof(u32);
		if (!buffer_read_u32_be(initramfs_buff, cursor, &headerOUT->size))
			return ERR_INVALID_MEMORY_REGION;
		cursor += sizeof(u32);
		u64 file_name_size = 0; // This is of type u8 but buffer_t interface wants u64
		if (!buffer_read_u8(initramfs_buff, cursor, (u8*)&file_name_size))
			return ERR_INVALID_MEMORY_REGION;
		cursor += sizeof(u8);

		buffer_t target_filename_buff = make_buffer(filename, strlen(filename));
		buffer_t filename_buff = make_buffer(initramfs_buff.data + cursor, file_name_size);
		cursor += file_name_size;

		if (buffer_memcmp(target_filename_buff, filename_buff) == 0) {
			if (headerOUT->offset + headerOUT->size > initramfs_buff.size)
				return ERR_OUT_OF_BOUNDS;
			return ERR_NONE;
		}
	}
	headerOUT->size = 0;
	headerOUT->offset = 0;
	return ERR_NOT_FOUND;
}

error_t hal_initramfs_read(const char* filename, buffer_t* buffOUT) {
	buffer_t initramfs_buff;
	error_t err = find_initramfs_buffer(&initramfs_buff);
	if (err)
		return err;
	err = validate(initramfs_buff);
	if (err)
		return err;
	file_header_t file_header;
	err = find_file_by_name(filename, initramfs_buff, &file_header);
	if (err)
		return err;
	*buffOUT = make_buffer(initramfs_buff.data + file_header.offset, file_header.size);
	return ERR_NONE;
}
