#include <drivers/console.h>
#include <drivers/dt/dt.h>
#include <drivers/dt/dt_parser.h>
#include <drivers/dt/dt_props.h>
#include <drivers/uart.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}
/*
static void sbi_put_u32(u32 num) {
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
*/

static void sbi_put_u64(u64 num) {
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
	sbi_puts("Initializing DT...\n");

	u32 blob_size = read_be32((const u8*)fdt + 4);

	if (dt_init(fdt, blob_size) < 0) {
		sbi_puts("DT_INIT FAILED\n");
		return;
	}

	sbi_puts("\nDT init OK\n");

	struct dt_node* root = dt_get_root();
	if (!root) {
		sbi_puts("No DT root\n");
		return;
	}

	sbi_puts("Root node exists\n");

	// Try to find the uart node
	const char* uart_path = "/soc/serial@10000000";
	struct dt_node* uart = dt_find_node(uart_path);
	if (!uart) {
		sbi_puts("UART node not found\n");
	} else {
		sbi_puts("Found UART node: ");
		sbi_puts(uart->name);
		sbi_puts("\n");

		// Read properties
		u64 base;
		if (dt_prop_read_u64(uart, "reg", &base) == 0) {
			sbi_puts("UART base: ");
			sbi_put_u64(base);
		} else {
			sbi_puts("\"reg\" prop missing or invalid\n");
		}
	}

	// Cleanup
	dt_cleanup();
	sbi_puts("\nDT cleaned up\n");
}
