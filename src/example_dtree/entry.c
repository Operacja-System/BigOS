#include <debug/debug_stdio.h>
#include <dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

void main([[maybe_unused]] u32 hartid, const void* fdt) {
	fdt_t fdt_obj;

	// Ignoring further errors for brevity
	error_t error = dt_init(fdt, &fdt_obj);
	if (error != ERR_NONE) {
		DEBUG_PRINTF("DT_INIT FAILED %d", error);
		return;
	}

	u32 main_node;
	error = dt_get_node_by_path(&fdt_obj, "/cpus", &main_node);
	u32 child;
	error = dt_get_node_child(&fdt_obj, main_node, &child);
	u32 sibling;
	error = dt_get_node_sibling(&fdt_obj, main_node, &sibling);

	DEBUG_PRINTF("Main node: %u\n", main_node);
	buffer_t buff;
	const char* name;
	error = dt_get_node_name(&fdt_obj, child, &buff);
	name = buff.data;
	DEBUG_PRINTF("Child: %s\n", name);
	error = dt_get_node_name_ptr(&fdt_obj, sibling, &name);
	DEBUG_PRINTF("Sibling: %s\n", name);

	u32 main_prop;
	error = dt_get_prop_by_name(&fdt_obj, main_node, "#address-cells", &main_prop);
	buffer_t buf;
	error = dt_get_prop_buffer(&fdt_obj, main_prop, &buf);

	const char* prop_name;
	error = dt_get_prop_name(&fdt_obj, main_prop, &buff);
	prop_name = buff.data;
	u32 prop_val;
	if (!buffer_read_u32_be(buf, 0, &prop_val)) {
		DEBUG_PRINTF("Bad read from %s\n", prop_name);
		return;
	}

	DEBUG_PRINTF("Main node prop: %s: %u\n", prop_name, prop_val);

	u32 first_prop;
	error = dt_get_first_prop(&fdt_obj, main_node, &first_prop);
	u32 next_prop;
	error = dt_get_next_prop(&fdt_obj, first_prop, &next_prop);

	error = dt_get_prop_name_ptr(&fdt_obj, first_prop, &prop_name);
	DEBUG_PRINTF("First prop: %s\n", prop_name);
	error = dt_get_prop_name_ptr(&fdt_obj, next_prop, &prop_name);
	DEBUG_PRINTF("Next prop: %s\n", prop_name);

	fdt_rsv_entry entry;
	error = dt_get_rsv_mem_entry(&fdt_obj, 0, &entry);

	if (error != ERR_NONE) {
		// To show that there's only one (terminating) entry in the rsvmap
		DEBUG_PRINTF("%u %u\n", fdt_obj.rsvmap_off, fdt_obj.struct_off);
		DEBUG_PRINTF("Failed to get reserved memory entry: %d\n", error);
		return;
	}
}
