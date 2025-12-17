#ifndef BIGOS_KERNEL_LOGGING_KLOG
#define BIGOS_KERNEL_LOGGING_KLOG

#include <stdbigos/types.h>

typedef enum {
	KLSL_ERROR = 0,
	KLSL_WARNING = 1,
	KLSL_NOTE = 2,
	KLSL_TRACE = 3,
} klog_severity_level_t;

void klog_indent_increase();
void klog_indent_decrease();

[[gnu::format(printf, 2, 3)]]
void klog(klog_severity_level_t loglvl, const char* fmt, ...);

[[gnu::format(printf, 2, 3)]]
void klogln(klog_severity_level_t loglvl, const char* fmt, ...);

typedef enum : u8 {
	KLRF_END_BLOCK = 0b00000001,
	KLRF_TRACE_ERR = 0b00000010,
} klog_return_flag_t;

#ifndef __LOGLVL__
	#define __LOGLVL__ 0
#endif

#if __LOGLVL__ >= 0
	#define KLOG_ERROR(fmt, ...)   klog(KLSL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_ERROR(fmt, ...) klog(KLSL_ERROR, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
	#define KLOG_ERROR(fmt, ...)
	#define KLOGLN_ERROR(fmt, ...)
#endif
#if __LOGLVL__ >= 1
	#define KLOG_WARNING(fmt, ...)   klog(KLSL_WARNING, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_WARNING(fmt, ...) klog(KLSL_WARNING, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
	#define KLOG_WARNING(fmt, ...)
	#define KLOGLN_WARNING(fmt, ...)
#endif
#if __LOGLVL__ >= 2
	#define KLOG_NOTE(fmt, ...)     klog(KLSL_NOTE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_NOTE(fmt, ...)   klog(KLSL_NOTE, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
	#define KLOG_INDENT_BLOCK_START klog_indent_increase()
	#define KLOG_INDENT_BLOCK_END   klog_indent_decrease()
#else
	#define KLOG_NOTE(fmt, ...)
	#define KLOGLN_NOTE(fmt, ...)
	#define KLOG_INDENT_BLOCK_START
	#define KLOG_INDENT_BLOCK_END
#endif
#if __LOGLVL__ >= 3
	#define KLOG_TRACE(fmt, ...)   klog(KLSL_TRACE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_TRACE(fmt, ...) klog(KLSL_TRACE, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
	#define KLOG_TRACE(fmt, ...)
	#define KLOGLN_TRACE(fmt, ...)
#endif

#define KLOG_DO_RETURN(ret, flag)                                                   \
	do {                                                                            \
		if ((flag) & KLRF_END_BLOCK) {                                              \
			KLOG_INDENT_BLOCK_END;                                                  \
		}                                                                           \
		if ((flag) & KLRF_TRACE_ERR) {                                              \
			KLOGLN_TRACE("Returned error: %u at %s:%u", (ret), __FILE__, __LINE__); \
		}                                                                           \
		return (ret);                                                               \
	} while (0)

#endif
