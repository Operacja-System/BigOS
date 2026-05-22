#ifndef HAL_SBI_UTILS
#define HAL_SBI_UTILS

#include <libcore/error.h>

#include "sbi.h"

static inline error_t sbi_map_error(sbi_error_t err) {
	switch (err) {
	case SBI_SUCCESS:           return ERR_NONE;
	case SBI_ERR_INVALID_PARAM: return ERR_BAD_ARG;
	case SBI_ERR_NOT_SUPPORTED: return ERR_NOT_IMPLEMENTED;
	default:                    return ERR_NOT_VALID;
	}
}

#endif // !HAL_SBI_UTILS
