#include <drivers/console.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_props.h>
#include <drivers/uart.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

[[maybe_unused]] static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

[[maybe_unused]] static void sbi_put_u32(u32 num) {
	char buf[11];
	int i = 10;
	buf[i] = '\0';
	i--;
	if (num == 0) {
		sbi_puts("0");
		return;
	}

	while (num && i >= 0) {
		buf[i] = (num % 10 + '0');
		i--;
		num /= 10;
	}
	sbi_puts(&buf[i + 1]);
}

[[maybe_unused]] static void sbi_put_u64(u64 num) {
	char buf[21];
	int i = 20;
	buf[i] = '\0';
	i--;
	if (num == 0) {
		sbi_puts("0");
		return;
	}

	while (num && i >= 0) {
		buf[i] = (num % 10 + '0');
		i--;
		num /= 10;
	}
	sbi_puts(&buf[i + 1]);
}

void main([[maybe_unused]] u32 hartid, const void* fdt) {
	u32 blob_size = read_be32((const u8*)fdt + 4);

	if (dt_init(fdt, blob_size) < 0) {
		sbi_puts("DT_INIT FAILED\n");
		return;
	}

	struct dt_node* root = dt_get_root();
	if (!root) {
		sbi_puts("GETTING ROOT FAILED\n");
		return;
	}

	// Showcasing that values and finding work
	const char* uart_path = "/soc/serial@10000000";
	struct dt_node* uart = dt_node_find(uart_path);
	if (!uart) {
		sbi_puts("UART node not found\n");
	} else {
		sbi_puts("Found UART node: ");
		sbi_puts(dt_node_get_name(uart));
		sbi_puts("\n");

		u64 base;
		if (dt_prop_read_u64(uart, "reg", &base) == 0) {
			sbi_puts("UART base: ");
			sbi_put_u64(base);
			sbi_puts("\n");
		} else {
			sbi_puts("\"reg\" prop missing or invalid\n");
		}
	}

	// Showcasing that the tree works
	dt_print_tree(root, 0);

	dt_cleanup();
}
