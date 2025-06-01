#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_props.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

// The nature of this main necessitates compilation in debug preset
void main([[maybe_unused]] u32 hartid, const void* fdt) {
	if (dt_init(fdt, ENDIAN_LITTLE) < 0) {
		DEBUG_PRINTF("DT_INIT FAILED");
		return;
	}

	dt_node_t* root = dt_get_root();
	if (!root) {
		DEBUG_PRINTF("GETTING ROOT FAILED");
		return;
	}

	// Showcasing that values and finding work
	const char* uart_path = "/soc/serial@10000000";
	dt_node_t* uart = dt_node_find(uart_path);
	if (!uart) {
		DEBUG_PRINTF("UART node not found");
	} else {
		DEBUG_PRINTF("Found UART node: %s\n", dt_node_get_name(uart));

		u64 base;
		if (dt_prop_read_u64(uart, "reg", &base) == 0) {
			DEBUG_PRINTF("UART base: %ld\n", base);
		} else {
			DEBUG_PRINTF("\"reg\" prop missing or invalid\n");
		}
	}

	// Showcasing that the tree works
	dt_print_tree(root, 0);

	dt_cleanup();
}
