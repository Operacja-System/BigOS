#include <stdbigos/error.h>

const char* get_error_msg(error_t err) {
	switch(err) {
	case ERR_NONE:							  return "";
	case ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED: return "Virtual memory scheme not implemented. Implemented: Sv48";
	case ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED:	  return "Virtual memory scheme not supported by the hardware";
	case ERR_ASID_NOT_SUPPORTED:			  return "Address space identifiers not supported by the hardware";
	case ERR_INVALID_PAGE_SIZE:				  return "Page size is not supported by the active virtual memory scheme";
	case ERR_PAGE_TABLE_DOESNT_EXIST:		  return "Page table already exists";
	case ERR_PAGE_TABLE_ALREADY_EXISTS:		  return "Page table doesn't exist";
	default:								  return "Error message was not provided";
	}
}
