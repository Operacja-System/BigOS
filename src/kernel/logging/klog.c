#include "klog.h"

#include <stdarg.h>
#include <stdbigos/stdio.h>
#include <stdbigos/types.h>

#include "debug/debug_stdio.h"

static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};
static const char* g_overflow_warning = "[WARNING]Some of the logs were lost due to overflow";

#ifdef __DEBUG__
	#define RING_BUF_SIZE (1 << 20)
#else
	#define RING_BUF_SIZE (1 << 16)
#endif // !__DEBUG__

#define TEMP_BUF_SIZE (1 << 12)

typedef struct {
	char ring[RING_BUF_SIZE];
	i32 read;
	i32 write;
	bool full;
	bool overflown;
	void (*uart_tx)(char c);
} ring_buffer_t;

static ring_buffer_t g_buffer;

static ring_buffer_t* get_buffer() {
	return &g_buffer;
}

static void put_char(ring_buffer_t* rb, const char c) {
	if (c == '\0')
		return; // Null characters are not suppossed to be logged.
	rb->ring[rb->write++] = c;
	rb->write %= RING_BUF_SIZE;
	if (rb->full) {
		rb->read = (rb->read + 1) % RING_BUF_SIZE;
		rb->overflown = true;
	}
	rb->full = rb->read == rb->write;
	if (rb->uart_tx)
		rb->uart_tx(c);
}

static void put_string(ring_buffer_t* rb, const char* str) {
	size_t it = 0;
	while (str[it]) put_char(rb, str[it++]);
}

static void put_overflow_warning(ring_buffer_t* rb) {
	size_t it = 0;
	i32 temp_write = rb->write;
	while (g_overflow_warning[it]) {
		rb->ring[temp_write++] = g_overflow_warning[it++];
		temp_write %= RING_BUF_SIZE;
	}
}

static void put_msg(ring_buffer_t* rb, const char* msg) {
	put_string(rb, msg);
	if (rb->overflown)
		put_overflow_warning(rb);
}

char ring_buffer_get_char() {
	ring_buffer_t* rb = get_buffer();
	if (rb->read == rb->write && !rb->full)
		return 0;
	const char ret = rb->ring[rb->read++];
	rb->read %= RING_BUF_SIZE;
	rb->full = false;
	rb->overflown = false;
	return ret;
}

void ring_buffer_flush_to_uart() {
	ring_buffer_t* rb = get_buffer();
	if (!rb->uart_tx)
		return;
	char c;
	while ((c = ring_buffer_get_char())) rb->uart_tx(c);
}

void ring_buffer_set_uart_tx_function(void (*uart_tx)(char c), bool flush) {
	ring_buffer_t* rb = get_buffer();
	rb->uart_tx = uart_tx;
	if (flush)
		ring_buffer_flush_to_uart();
}

void klog_indent_increase() {
	++g_indent_level;
}

void klog_indent_decrease() {
	if (g_indent_level != 0)
		--g_indent_level;
}

static void klogv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	char temp_buffer[TEMP_BUF_SIZE];

	if (loglvl > KLSL_TRACE) {
		KLOGLN_ERROR("Invalid loglvl passed to klog");
		loglvl = KLSL_ERROR;
	}

	size_t written = 0;
	for (u32 i = 0; i < g_indent_level && written < TEMP_BUF_SIZE - 1; i++) temp_buffer[written++] = ' ';

	const char* prefix = g_prefixes[loglvl];
	for (size_t i = 0; prefix[i] && written < TEMP_BUF_SIZE - 1; i++) temp_buffer[written++] = prefix[i];

	va_list copy;
	va_copy(copy, va);
	int n = vsnprintf(temp_buffer + written, TEMP_BUF_SIZE - written, fmt, copy);
	va_end(copy);

	// stbsp_vsnprintf doesn't return errors, so just check for truncation
	if (n < 0)
		n = 0;
	written += (size_t)n;

	if (written >= TEMP_BUF_SIZE)
		written = TEMP_BUF_SIZE - 1;
	temp_buffer[written] = '\0';

	ring_buffer_t* rb = get_buffer();
	put_msg(rb, temp_buffer);

	// DEBUG_PUTGAP(g_indent_level);
	// DEBUG_PRINTF("%s ", g_prefixes[loglvl]);
	// DEBUG_VPRINTF(fmt, va);
}

static void kloglnv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	klogv(loglvl, fmt, va);
	put_msg(&g_buffer, "\n");
	// DEBUG_PUTC('\n');
}

void klog(klog_severity_level_t loglvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	klogv(loglvl, fmt, args);
	va_end(args);
}

void klogln(klog_severity_level_t loglvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	kloglnv(loglvl, fmt, args);
	va_end(args);
}
