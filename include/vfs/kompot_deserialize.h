#ifndef _VFS_DESERIALIZE_
#define _VFS_DESERIALIZE_

#include <stdbigos/types.h>
#include <vfs/kompot.h>

[[nodiscard]] size_or_err_t kompot_deserialize_message_metadata(const pstring_t* data,
                                                                KompotMessageMetadata_t* message);

[[nodiscard]] size_or_err_t kompot_deserialize_request_version(const pstring_t* data, KompotRequestVersion_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_version(const pstring_t* data,
                                                                KompotResponseVersion_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_connect(const pstring_t* data, KompotRequestConnect_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_connect(const pstring_t* data,
                                                                KompotResponseConnect_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_walk(const pstring_t* data, pstring_t* buff,
                                                            KompotRequestWalk_t** request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_walk(const pstring_t* data, KompotResponseWalk_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_stat(const pstring_t* data, KompotRequestStat_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_stat(const pstring_t* data, KompotResponseStat_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_create(const pstring_t* data, pstring_t* buff,
                                                              KompotRequestCreate_t** request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_create(const pstring_t* data, KompotResponseCreate_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_delete(const pstring_t* data, KompotRequestDelete_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_delete(const pstring_t* data, KompotResponseDelete_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_open(const pstring_t* data, KompotRequestOpen_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_open(const pstring_t* data, KompotResponseOpen_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_read(const pstring_t* data, KompotRequestRead_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_read(const pstring_t* data, pstring_t* buff,
                                                             KompotResponseRead_t** response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_write(const pstring_t* data, pstring_t* buff,
                                                             KompotRequestWrite_t** request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_write(const pstring_t* data, KompotResponseWrite_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_wstat(const pstring_t* data, KompotRequestWstat_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_wstat(const pstring_t* data, KompotResponseWstat_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_request_close(const pstring_t* data, KompotRequestClose_t* request);
[[nodiscard]] size_or_err_t kompot_deserialize_response_close(const pstring_t* data, KompotResponseClose_t* response);

[[nodiscard]] size_or_err_t kompot_deserialize_response_error(const pstring_t* data, pstring_t* buff,
                                                              KompotResponseError_t** response);

#endif
