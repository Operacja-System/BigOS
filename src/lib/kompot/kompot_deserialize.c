#include <kompot/kompot.h>
#include <kompot/kompot_deserialize.h>
#include <stdbigos/error.h>
#include <stdbigos/meta/err_x_t.h>
#include <stdbigos/pstring.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

// NOTE: In future to achieve better performace we might want to attempt to calculate structure size first
//       and only then deserialize without checks in between.

size_or_err_t kompot_deserialize_message_metadata(const pstring_t* data, KompotMessageMetadata_t* message) {
	if (data == NULL || message == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &message->size));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, (u16*)&message->tag));
	// request_type and response_type are unioned, they occupy the same space
	ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, (u16*)&message->request_type));
	return (size_or_err_t){.val = sizeof(KompotMessageMetadata_t)};
}

size_or_err_t kompot_deserialize_request_version(const pstring_t* data, KompotRequestVersion_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->max_size));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->version));
	return (size_or_err_t){.val = sizeof(KompotRequestVersion_t)};
}

size_or_err_t kompot_deserialize_response_version(const pstring_t* data, KompotResponseVersion_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &response->max_size));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &response->version));
	return (size_or_err_t){.val = sizeof(KompotResponseVersion_t)};
}

size_or_err_t kompot_deserialize_request_connect(const pstring_t* data, KompotRequestConnect_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, 0, &request->uid));
	return (size_or_err_t){.val = sizeof(KompotRequestConnect_t)};
}

size_or_err_t kompot_deserialize_response_connect(const pstring_t* data, KompotResponseConnect_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, 0, &response->cursor.ino));
	return (size_or_err_t){.val = sizeof(KompotResponseConnect_t)};
}

// Fills the struct, places the path in buff after request.
size_or_err_t kompot_deserialize_request_walk(const pstring_t* data, pstring_t* buff, KompotRequestWalk_t** request) {
	if (data == NULL || buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}
	// Verify whether there enough space in the buffer for fixed fields
	if (buff->len < sizeof(KompotRequestWalk_t)) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	KompotRequestWalk_t* walk_request = (KompotRequestWalk_t*)buff->data;
	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &walk_request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &walk_request->uid));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &walk_request->walk_count));

	// Verify whether we wil be able to fit all pstrings in buff
	const size_t struct_size = sizeof(KompotRequestWalk_t) + (walk_request->walk_count * sizeof(pstring_t));
	if (buff->len < struct_size) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	// Represents space where we will place path
	pstring_t path_buff = {.data = buff->data + struct_size, .len = buff->len - struct_size};

	for (u16 i = 0; i < walk_request->walk_count; i++) {
		u16 curr_path_len = 0;
		offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &curr_path_len));
		// No space for data in buffer
		if (path_buff.len < curr_path_len) {
			return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
		}

		walk_request->walk_path[i] = (pstring_t){.data = path_buff.data, .len = curr_path_len};
		pstring_t curr_path = {.data = data->data + offset, .len = data->len - offset};

		// Not enough data provided
		if (curr_path.len < curr_path_len) {
			return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
		}

		offset += ERRX_UNWRAP(pstring_memmove(&walk_request->walk_path[i], &curr_path, curr_path_len));
		path_buff.data += curr_path_len;
		path_buff.len -= curr_path_len;
	}

	*request = walk_request;
	return (size_or_err_t){.val = path_buff.data - buff->data};
}

size_or_err_t kompot_deserialize_response_walk(const pstring_t* data, KompotResponseWalk_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, 0, &response->cursor.ino));
	return (size_or_err_t){.val = sizeof(KompotResponseWalk_t)};
}

size_or_err_t kompot_deserialize_request_stat(const pstring_t* data, KompotRequestStat_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->cursor.ino));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->uid));
	return (size_or_err_t){.val = sizeof(KompotRequestStat_t)};
}

size_or_err_t kompot_deserialize_response_stat(const pstring_t* data, KompotResponseStat_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	// TODO: Implement once stat structure is created
	ERRX_UNWRAP_OR_RETURN(pstring_read_u8(data, 0, &response->stat));
	return (size_or_err_t){.val = sizeof(KompotResponseStat_t)};
}

size_or_err_t kompot_deserialize_request_create(const pstring_t* data, pstring_t* buff,
                                                KompotRequestCreate_t** request) {
	if (data == NULL || buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	KompotRequestCreate_t* create_request = (KompotRequestCreate_t*)buff->data;
	if (buff->len < sizeof(KompotRequestCreate_t)) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &create_request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &create_request->uid));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &create_request->create_info.permissions));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &create_request->create_info.file_type));

	u16 name_len = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &name_len));

	// Not enough space for data
	if (buff->len < sizeof(KompotRequestCreate_t) + name_len) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	create_request->name = (pstring_t){.data = buff->data + sizeof(KompotRequestCreate_t), .len = name_len};
	pstring_t name = {
	    .data = data->data + offset,
	    .len = data->len - offset,
	};

	// Not enough data provided
	if (name.len < name_len) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	ERRX_UNWRAP(pstring_memmove(&create_request->name, &name, name_len));
	*request = create_request;
	return (size_or_err_t){.val = sizeof(KompotRequestCreate_t) + name_len};
}

size_or_err_t kompot_deserialize_response_create(const pstring_t* data, KompotResponseCreate_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, 0, &response->cursor.ino));
	return (size_or_err_t){.val = sizeof(KompotResponseCreate_t)};
}

size_or_err_t kompot_deserialize_request_delete(const pstring_t* data, KompotRequestDelete_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->cursor.ino));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->uid));
	return (size_or_err_t){.val = sizeof(KompotRequestDelete_t)};
}

size_or_err_t kompot_deserialize_response_delete(const pstring_t* data, KompotResponseDelete_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	// No data to deserialize for delete response
	return (size_or_err_t){.val = 0};
}

size_or_err_t kompot_deserialize_request_open(const pstring_t* data, KompotRequestOpen_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->uid));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u8(data, offset, &request->mode));
	return (size_or_err_t){.val = sizeof(KompotRequestOpen_t)};
}

size_or_err_t kompot_deserialize_response_open(const pstring_t* data, KompotResponseOpen_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &response->file_handle));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &response->max_atomic_op));
	return (size_or_err_t){.val = sizeof(KompotResponseOpen_t)};
}

size_or_err_t kompot_deserialize_request_read(const pstring_t* data, KompotRequestRead_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->offset));
	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &request->count));
	return (size_or_err_t){.val = sizeof(KompotRequestRead_t)};
}

size_or_err_t kompot_deserialize_response_read(const pstring_t* data, pstring_t* buff,
                                               KompotResponseRead_t** response) {
	if (data == NULL || buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	KompotResponseRead_t* read_response = (KompotResponseRead_t*)buff->data;
	if (buff->len < sizeof(KompotResponseRead_t)) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &read_response->count));

	// Verify there is enough space in buffer for data
	if (buff->len < sizeof(KompotResponseRead_t) + read_response->count) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	// Verify there is enough data provided
	if (data->len < sizeof(KompotResponseRead_t) + read_response->count) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	memmove(read_response->data, data->data + offset, read_response->count);
	*response = read_response;
	return (size_or_err_t){.val = sizeof(KompotResponseRead_t) + read_response->count};
}

size_or_err_t kompot_deserialize_request_write(const pstring_t* data, pstring_t* buff, KompotRequestWrite_t** request) {
	if (data == NULL || buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	KompotRequestWrite_t* write_request = (KompotRequestWrite_t*)buff->data;
	if (buff->len < sizeof(KompotRequestWrite_t)) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &write_request->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &write_request->offset));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, offset, &write_request->count));

	// Verify there is enough space in buffer for data
	if (buff->len < sizeof(KompotRequestWrite_t) + write_request->count) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	// Verify there is enough data provided
	if (data->len < sizeof(KompotRequestWrite_t) + write_request->count) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	memmove(write_request->data, data->data + offset, write_request->count);
	*request = write_request;
	return (size_or_err_t){.val = sizeof(KompotRequestWrite_t) + write_request->count};
}

size_or_err_t kompot_deserialize_response_write(const pstring_t* data, KompotResponseWrite_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u32(data, 0, &response->count));
	return (size_or_err_t){.val = sizeof(KompotResponseWrite_t)};
}

size_or_err_t kompot_deserialize_request_wstat(const pstring_t* data, KompotRequestWstat_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &request->file_handle));
	u64 stat_size;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, offset, &stat_size));
	// TODO: Implement once stat structure is created
	return (size_or_err_t){.val = sizeof(KompotRequestWstat_t)};
}

size_or_err_t kompot_deserialize_response_wstat(const pstring_t* data, KompotResponseWstat_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	// No data to deserialize for wstat response
	return (size_or_err_t){.val = 0};
}

size_or_err_t kompot_deserialize_request_close(const pstring_t* data, KompotRequestClose_t* request) {
	if (data == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	ERRX_UNWRAP_OR_RETURN(pstring_read_u64(data, 0, &request->file_handle));
	return (size_or_err_t){.val = sizeof(KompotRequestClose_t)};
}

size_or_err_t kompot_deserialize_response_close(const pstring_t* data, KompotResponseClose_t* response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	// No data to deserialize for close response
	return (size_or_err_t){.val = 0};
}

size_or_err_t kompot_deserialize_response_error(const pstring_t* data, pstring_t* buff,
                                                KompotResponseError_t** response) {
	if (data == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	KompotResponseError_t* error_response = (KompotResponseError_t*)buff->data;
	size_t offset = 0;
	u16 name_len = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_read_u16(data, offset, &name_len));

	// Verify there is enough space left in buffer
	if (buff->len < sizeof(KompotResponseError_t) + name_len) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	// Verify there is enough data
	if (data->len < sizeof(KompotResponseError_t) + name_len) {
		return (size_or_err_t){.err = ERR_BUFFER_TOO_SHORT};
	}

	error_response->error_name = (pstring_t){
	    .data = buff->data + sizeof(KompotResponseError_t),
	    .len = name_len,
	};
	pstring_t name = {.data = data->data + offset, .len = name_len};

	ERRX_UNWRAP(pstring_memcpy(&error_response->error_name, &name));
	*response = error_response;
	return (size_or_err_t){.val = sizeof(KompotResponseError_t) + name_len};
}
