#include "klog.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbigos/types.h>

// NOTE: In the future this lib should manage logging (formating and outputing to the screen or uart) on its own.
//  Right now I will leave it as is, will change once the basic kernel at least runs.
#include "debug/debug_stdio.h"

// static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};
static const char* overwrite_warning = "[WARNING]Some of the logs were lost due to too small size of buffer";

#ifdef __DEBUG__
#define RING_BUF_SIZE 1<<20
#else
#define RING_BUF_SIZE 1<<16
#endif // !__DEBUG__

#define TEMP_BUF_SIZE 1<<12

typedef struct {
	char ring[RING_BUF_SIZE];
	i32 read;
	i32 write;
	bool full;
} ring_buffer;

static ring_buffer buffer;

size_t available_space(const ring_buffer* buf) {
    if (buf->full) return 0;
    if (buf->write >= buf->read)
        return RING_BUF_SIZE - (buf->write - buf->read);
    else
        return buf->read - buf->write;
}

void put_char(ring_buffer* buf, char c){
	buf->ring[buf->write++] = c;
	buf->write %= RING_BUF_SIZE;
	if(buf->full){
		buf->read = (buf->read + 1) % RING_BUF_SIZE;
	}
	buf->full = buf->read == buf->write;
}

void put_string(ring_buffer* buf, const char* str){
	size_t it = 0;
	while(str[it]){
		put_char(buf, str[it++]);
	}
}

void put_msg(ring_buffer* buf, const char* msg, int msg_len){
	if ((size_t)msg_len > available_space(buf))
        put_string(buf, overwrite_warning);
    put_string(buf, msg);
}

char get_char(ring_buffer* buf){
	if(buf->read == buf->write && !buf->full)
		return 0;
	char ret = buf->ring[buf->read++];
	buf->read %= RING_BUF_SIZE;
	buf->full = false;
	return ret;
}

// void klog_indent_increase() {
// 	++g_indent_level;
// }

// void klog_indent_decrease() {
// 	if (g_indent_level != 0)
// 		--g_indent_level;
// }

static void klogv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	static char temp_buffer[TEMP_BUF_SIZE];

	if (loglvl > KLSL_TRACE) {
		KLOGLN_ERROR("Invalid loglvl passed to klog");
		loglvl = KLSL_ERROR;
	}

	const char* prefix = g_prefixes[loglvl];
    size_t prefix_len = 0;
    while (prefix[prefix_len]) ++prefix_len;

	for (size_t i=0;i<prefix_len;i++)
        temp_buffer[i] = prefix[i];

	va_list copy;
	va_copy(copy, va);
	int written = vsnprintf(temp_buffer + prefix_len, sizeof(temp_buffer) - prefix_len, fmt, copy);
	va_end(copy);

	// TODO: proper error handling
	if (written < 0) {
		// Format error
		written = 0;
	} else if ((size_t)written >= sizeof(temp_buffer)) {
		// Truncated
		written = sizeof(temp_buffer) - 1;
	}

	put_msg(&buffer, &temp_buffer, written);

	// DEBUG_PUTGAP(g_indent_level);
	// DEBUG_PRINTF("%s ", g_prefixes[loglvl]);
	// DEBUG_VPRINTF(fmt, va);
}

static void kloglnv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	klogv(loglvl, fmt, va);
	put_msg(&buffer, "\n", 1);
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
