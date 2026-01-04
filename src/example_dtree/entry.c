#include <debug/debug_stdio.h>
#include <dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

void main([[maybe_unused]] u32 hartid, const void* fdt) {
	int a;

	if ((a = dt_init(fdt)) < 0) {
		DEBUG_PRINTF("DT_INIT FAILED %d", a);
		return;
	}

	dt_node_t root = dt_get_root();
	if (!root) {
		DEBUG_PRINTF("GETTING ROOT FAILED");
		return;
	}

	const char* path = "/cpus";

	u32 main_node = dt_get_node_by_path(fdt, path);

	DEBUG_PRINTF("Main node: %u\n", main_node);

	u32 child = dt_get_node_child(fdt, main_node);

	char child_name[64];

	dt_get_node_name(fdt, child, child_name, 64);

	DEBUG_PRINTF("Child: %s\n", child_name);

	char sibling_name[64];

	u32 sibling = dt_get_node_sibling(fdt, main_node);

	dt_get_node_name(fdt, sibling, sibling_name, 64);

	DEBUG_PRINTF("Sibling: %s\n", sibling_name);

	u32 main_prop = dt_get_prop_by_name(fdt, main_node, "#address-cells");

	char m_prop_name[64];

	dt_get_prop_name(fdt, main_prop, m_prop_name, 64);

	DEBUG_PRINTF("Main node prop: %s\n", m_prop_name);

	buffer_t buf = dt_get_prop_buffer(fdt, main_prop);

	u32 prop_val;
	(void)buffer_read_u32_be(buf, 0, &prop_val);

	DEBUG_PRINTF("Prop val:%u\n", prop_val);

	u32 first_prop = dt_get_first_prop(fdt, main_node);

	char f_prop_name[64];

	dt_get_prop_name(fdt, first_prop, f_prop_name, 64);

	DEBUG_PRINTF("First prop: %s\n", f_prop_name);

	u32 next_prop = dt_get_next_prop(fdt, main_prop);

	char n_prop_name[64];

	dt_get_prop_name(fdt, next_prop, n_prop_name, 64);

	DEBUG_PRINTF("Next prop: %s\n", n_prop_name);
}
