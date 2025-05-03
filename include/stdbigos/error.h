#ifndef _STDBIGOS_ERROR_H_
#define _STDBIGOS_ERROR_H_

typedef enum {
	ERR_NONE = 0,
	ERR_INVALID_ARGUMENT,
} error_t;

const char* get_error_msg(error_t err);

#endif //_STDBIGOS_ERROR_H_
