#include <kompot/kompot.h>
#include <kompot/kompot_serialize.h>
#include <stdbigos/error.h>
#include <stdbigos/meta/err_x_t.h>
#include <stdbigos/pstring.h>
#include <stdbigos/types.h>

// Note: This can be sped up by checking whether buff is large enough at the start of serializer

size_or_err_t kompot_serialize_message_metadata(const KompotMessageMetadata_t* metadata, pstring_t* buff) {
	if (buff == NULL || metadata == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, metadata->size));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, metadata->tag));
	// request_type and response_type are unioned, they occupy the same space
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, metadata->request_type));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_version(const KompotRequestVersion_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->max_size));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->version));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_version(const KompotResponseVersion_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, response->max_size));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, response->version));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_connect(const KompotRequestConnect_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_connect(const KompotResponseConnect_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, response->cursor.ino));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_walk(const KompotRequestWalk_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, request->walk_count));

	for (u16 i = 0; i < request->walk_count; i++) {
		const pstring_t* walk_path = &request->walk_path[i];
		offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, (u16)walk_path->len));
		offset += ERRX_UNWRAP_OR_RETURN(pstring_cat(buff, offset, walk_path));
	}

	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_walk(const KompotResponseWalk_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, response->cursor.ino));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_stat(const KompotRequestStat_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_stat(const KompotResponseStat_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, 0));
	// TODO: Implement once stat structure is created
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_create(const KompotRequestCreate_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, request->create_info.permissions));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, request->create_info.file_type));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, (u16)request->name.len));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_cat(buff, offset, &request->name));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_create(const KompotResponseCreate_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, response->cursor.ino));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_delete(const KompotRequestDelete_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_delete(const KompotResponseDelete_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	// No data to serialize for delete response
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_open(const KompotRequestOpen_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->cursor.ino));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->uid));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u8(buff, offset, request->mode));
	// Currently we do not use last 24bits
	return (size_or_err_t){.val = offset + 3};
}

size_or_err_t kompot_serialize_response_open(const KompotResponseOpen_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, response->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, response->max_atomic_op));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_read(const KompotRequestRead_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->offset));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->count));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_read(const KompotResponseRead_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, response->count));
	offset += ERRX_UNWRAP_OR_RETURN(
	    pstring_cat(buff, offset, &(pstring_t){.len = response->count, .data = (u8*)response->data}));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_write(const KompotRequestWrite_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->offset));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, request->count));
	offset += ERRX_UNWRAP_OR_RETURN(
	    pstring_cat(buff, offset, &(pstring_t){.len = request->count, .data = (u8*)request->data}));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_write(const KompotResponseWrite_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u32(buff, offset, response->count));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_wstat(const KompotRequestWstat_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->file_handle));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, 0));
	// TODO: Implement once stat structure is created
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_wstat(const KompotResponseWstat_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	// No data to serialize for wstat response
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_request_close(const KompotRequestClose_t* request, pstring_t* buff) {
	if (buff == NULL || request == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u64(buff, offset, request->file_handle));
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_close(const KompotResponseClose_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	// No data to serialize for close response
	return (size_or_err_t){.val = offset};
}

size_or_err_t kompot_serialize_response_error(const KompotResponseError_t* response, pstring_t* buff) {
	if (buff == NULL || response == NULL) {
		return (size_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}

	size_t offset = 0;
	offset += ERRX_UNWRAP_OR_RETURN(pstring_write_u16(buff, offset, (u16)response->error_name.len));
	offset += ERRX_UNWRAP_OR_RETURN(pstring_cat(buff, offset, &response->error_name));
	return (size_or_err_t){.val = offset};
}
