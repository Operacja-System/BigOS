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

	buffer_t child_name = dt_get_node_name(fdt, child);

	const char* c_name;
	(void)buffer_read_cstring(child_name, 0, &c_name);

	DEBUG_PRINTF("Child: %s\n", c_name);

	u32 sibling = dt_get_node_sibling(fdt, main_node);

	buffer_t sibling_name = dt_get_node_name(fdt, sibling);

	const char* s_name;
	(void)buffer_read_cstring(sibling_name, 0, &s_name);

	DEBUG_PRINTF("Sibling: %s\n", s_name);

	u32 main_prop = dt_get_prop_by_name(fdt, main_node, "#address-cells");

	buffer_t main_prop_name = dt_get_prop_name(fdt, main_prop);

	const char* m_prop;

	(void)buffer_read_cstring(main_prop_name, 0, &m_prop);

	DEBUG_PRINTF("Main node prop: %s\n", m_prop);

	buffer_t buf = dt_get_prop_buffer(fdt, main_prop);

	u32 prop_val;
	(void)buffer_read_u32_be(buf, 0, &prop_val);

	DEBUG_PRINTF("Prop val:%u\n", prop_val);

	u32 first_prop = dt_get_first_prop(fdt, main_node);

	buffer_t first_prop_name = dt_get_prop_name(fdt, first_prop);

	const char* f_prop;

	(void)buffer_read_cstring(first_prop_name, 0, &f_prop);

	DEBUG_PRINTF("First prop: %s\n", f_prop);

	u32 next_prop = dt_get_next_prop(fdt, main_prop);

	buffer_t next_prop_name = dt_get_prop_name(fdt, next_prop);

	const char* n_prop;

	(void)buffer_read_cstring(next_prop_name, 0, &n_prop);

	DEBUG_PRINTF("Next prop: %s\n", n_prop);
}
