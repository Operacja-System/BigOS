// Copy form frogrammer9/BigOs as we might want to use this

#include <stdbigos/error.h>

const char* get_error_msg(error_t err) {
	switch (err) {
	case ERR_NONE:             return "";
	case ERR_INVALID_ARGUMENT: return "Invalid argument";

	case ERR_MALLOC_FAILED: return "Malloc failed";

	case ERR_MT_MOUNTPOINT_EXSITS: return "Mount point already exists";
	case ERR_MT_UNKNOWN:           return "Unknown Mount Tree error";

	default: return "Error message was not provided";
	}
}
