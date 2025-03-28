#ifndef _STDBIGOS_ERROR_H_
#define _STDBIGOS_ERROR_H_

typedef enum {
	ERR_NONE,
	ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED,
} ERROR_t;

#define ERROR [[nodiscard]] ERROR_t

const char* get_error_msg(ERROR_t err); //TODO: implement

#endif //_STDBIGOS_ERROR_H_
