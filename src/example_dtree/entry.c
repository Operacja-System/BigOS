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

		buffer_t buffer = dt_prop_get_buffer(uart, "reg");
		if (buffer.error == BUF_ERR_OK) {
			// Or other easy conversion methods, up to user
			u64 val = read_be64(buffer.data);
			DEBUG_PRINTF("UART base: %lu\n", val);
		} else {
			DEBUG_PRINTF("\"reg\" prop missing or invalid\n");
		}
	}

// Showcasing that the tree works
#ifndef NDEBUG
	dt_print_tree(root, 0);
#endif

	dt_cleanup();
}
