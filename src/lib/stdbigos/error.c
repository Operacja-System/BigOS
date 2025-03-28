#include <stdbigos/error.h>

const char* get_error_msg(error_t err) {
	switch(err) {
	case ERR_NONE:							  return "";
	case ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED: return "Virtual memory scheme not implemented. Implemented: Sv48";
	case ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED:	  return "Virtual memory scheme not supported by the hardware";
	case ERR_ASID_NOT_SUPPORTED:			  return "Address space identifiers not supported by the hardware";
	default:								  return "Error message was not provided";
	}
}
