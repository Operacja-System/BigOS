#ifndef BIGOS_KERNEL_KLOG
#define BIGOS_KERNEL_KLOG

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

#define KLOG_ERROR(fmt, ...) klog(KLSL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)
#define __LOG_ERROR__
#define KLOGLN_ERROR(fmt, ...) klogln(KLSL_ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)
#if __LOGLVL__ >= 1
	#define KLOG_WARNING(fmt, ...)   klog(KLSL_WARNING, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_WARNING(fmt, ...) klogln(KLSL_WARNING, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define __LOG_WARNING__
#else
	#define KLOG_WARNING(fmt, ...)
	#define KLOGLN_WARNING(fmt, ...)
#endif
#if __LOGLVL__ >= 2
	#define KLOG_NOTE(fmt, ...)   klog(KLSL_NOTE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_NOTE(fmt, ...) klogln(KLSL_NOTE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define __ENABLE_LOG_INDENT__
	#define __LOG_NOTE__
#else
	#define KLOG_NOTE(fmt, ...)
	#define KLOGLN_NOTE(fmt, ...)
#endif
#if __LOGLVL__ >= 3
	#define KLOG_TRACE(fmt, ...)   klog(KLSL_TRACE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define KLOGLN_TRACE(fmt, ...) klogln(KLSL_TRACE, fmt __VA_OPT__(, ) __VA_ARGS__)
	#define __LOG_TRACE__
#else
	#define KLOG_TRACE(fmt, ...)
	#define KLOGLN_TRACE(fmt, ...)
#endif

#ifdef __ENABLE_LOG_INDENT__
	#define KLOG_INDENT_BLOCK_START klog_indent_increase()
	#define KLOG_INDENT_BLOCK_END   klog_indent_decrease()
	#define KLOG_END_BLOCK_AND_RETURN(x) \
		do {                             \
			klog_indent_decrease();      \
			return x;                    \
		} while (0)
#else
	#define KLOG_INDENT_BLOCK_START
	#define KLOG_INDENT_BLOCK_END
	#define KLOG_END_BLOCK_AND_RETURN(x) return x
#endif

#endif
