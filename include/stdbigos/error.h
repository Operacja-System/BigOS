#ifndef _STDBIGOS_ERROR_H_
#define _STDBIGOS_ERROR_H_

typedef enum {
	ERR_NONE,
	ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED,
	ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED,
	ERR_ASID_NOT_SUPPORTED,
} error_t;

const char* get_error_msg(error_t err);

#endif //_STDBIGOS_ERROR_H_
