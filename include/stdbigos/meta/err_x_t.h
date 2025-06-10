#ifndef STDBIGOS_META_ERR_X_T_H
#define STDBIGOS_META_ERR_X_T_H

#include <stdbigos/error.h>
#include <stdbigos/panic.h>

#define ERRX_STRUCT_DEFINE(name, type) \
	typedef struct {                   \
		error_t err;                   \
		type val;                      \
	} name##_t;

#define ERRX_FNS_DEFINE                                 \
	static inline name##_t name##_ok(type val) {        \
		return (name##_t){.err = ERR_NONE, .val = val}; \
	}                                                   \
	static inline name##_t name##_err(error_t err) {    \
		return (name##_t){.err = err};                  \
	}

#define ERRX_WRAPPER_T(name, type) ERRX_STRUCT_DEFINE(name##_##or_err, type)

#define ERRX_UNWRAP(x)                \
	({                                \
		typeof(x) __errt = x;         \
		if (__errt.err != ERR_NONE) { \
			PANIC_E(__errt.err);      \
		}                             \
		__errt.val;                   \
	})

#define ERRX_UNWRAP_OR_DEFAULT(x, default)               \
	({                                                   \
		typeof(x) __errt = x;                            \
		__errt.err == ERR_NONE ? __errt.val : (default); \
	})

#endif
