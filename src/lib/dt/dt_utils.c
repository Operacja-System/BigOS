#include <dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt_defines.h"

error_t dt_init(const void* fdt, fdt_t* obj) {
	if (fdt == nullptr || obj == nullptr)
		return ERR_BAD_ARG;

	obj->fdt_buffer = make_buffer(nullptr, 0);

	u32 magic = read_be32(fdt);
	if (magic != FDT_MAGIC)
		return ERR_NOT_VALID;

	u32 fdt_size = read_be32((u8*)fdt + FDT_OFF_TOTAL_SIZE);

	buffer_t fdt_buf = make_buffer(fdt, fdt_size);

	obj->fdt_buffer = fdt_buf;

	if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4)) {
		obj->fdt_buffer = make_buffer(nullptr, 0);
		return ERR_NOT_VALID;
	}

	u32 last_comp_version;

	if (!buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &obj->total_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &obj->struct_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &obj->strings_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_MEM_RSVMAP, &obj->rsvmap_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &obj->struct_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_VERSION, &obj->fdt_version) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_LAST_COMP_VERSION, &last_comp_version)) {
		obj->fdt_buffer = make_buffer(nullptr, 0);
		return ERR_NOT_VALID;
	}

	if (FDT_COMPATIBLE_VERSION < last_comp_version || FDT_COMPATIBLE_VERSION > obj->fdt_version) {
		obj->fdt_buffer = make_buffer(nullptr, 0);
		return ERR_NOT_VALID;
	}

	if (obj->struct_off + obj->struct_size > obj->total_size) {
		obj->fdt_buffer = make_buffer(nullptr, 0);
		return ERR_NOT_VALID;
	}

	obj->root_node = obj->struct_off;

	return ERR_NONE;
}

// Reset properties
void dt_reset(fdt_t* obj) {
	obj->fdt_buffer = make_buffer(nullptr, 0);
	obj->root_node = 0;
	obj->total_size = 0;
	obj->struct_off = 0;
	obj->strings_off = 0;
	obj->struct_size = 0;
	obj->fdt_version = 0;
}
