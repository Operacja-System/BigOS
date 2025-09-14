#ifndef _VFS_SERIALIZE_
#define _VFS_SERIALIZE_

#include <kompot/kompot.h>
#include <stdbigos/types.h>

[[nodiscard]] size_or_err_t kompot_serialize_message_metadata(const KompotMessageMetadata_t* metadata, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_version(const KompotRequestVersion_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_version(const KompotResponseVersion_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_connect(const KompotRequestConnect_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_connect(const KompotResponseConnect_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_walk(const KompotRequestWalk_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_walk(const KompotResponseWalk_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_stat(const KompotRequestStat_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_stat(const KompotResponseStat_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_create(const KompotRequestCreate_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_create(const KompotResponseCreate_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_delete(const KompotRequestDelete_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_delete(const KompotResponseDelete_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_open(const KompotRequestOpen_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_open(const KompotResponseOpen_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_read(const KompotRequestRead_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_read(const KompotResponseRead_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_write(const KompotRequestWrite_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_write(const KompotResponseWrite_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_wstat(const KompotRequestWstat_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_wstat(const KompotResponseWstat_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_request_close(const KompotRequestClose_t* request, pstring_t* buff);
[[nodiscard]] size_or_err_t kompot_serialize_response_close(const KompotResponseClose_t* response, pstring_t* buff);

[[nodiscard]] size_or_err_t kompot_serialize_response_error(const KompotResponseError_t* response, pstring_t* buff);

#endif
