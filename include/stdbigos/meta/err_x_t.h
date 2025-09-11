#ifndef STDBIGOS_META_ERR_X_T_H
#define STDBIGOS_META_ERR_X_T_H

#include <debug/panic.h>
#include <stdbigos/error.h>

#define ERRX_STRUCT_DEFINE(name, type) \
	typedef struct {                   \
		error_t err;                   \
		type val;                      \
	} name##_t;

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

#define ERRX_UNWRAP_OR_RETURN(x)      \
	({                                \
		typeof(x) __errt = x;         \
		if (__errt.err != ERR_NONE) { \
			return x;                 \
		}                             \
		__errt.val;                   \
	})

#endif
