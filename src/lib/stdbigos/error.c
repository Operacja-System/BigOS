#include <stdbigos/error.h>

const char* get_error_msg(error_t err) {
	switch(err) {
	case ERR_NONE:			   return "";
	case ERR_INVALID_ARGUMENT: return "Invalid argument";
	default:				   return "Error message was not provided";
	}
}
