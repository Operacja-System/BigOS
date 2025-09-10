// Defined here are structures for VFS FS Server Communication Protocol (KOMPOT)
// For more information see docs/vfs/Kompot.md

#ifndef _VFS_KOMPOT_
#define _VFS_KOMPOT_

#include <stdbigos/types.h>

// TODO: create proper authentication types.
//       This will be sufficient if we create an authentication server
typedef u32 uid_t;

/// KOMPOT Version
typedef u32 KompotVersion_t;

/// Unique tag for a request (response will have the same tag)
typedef u16 KompotMessageTag_t;

/// Abstract inode number representation
typedef u64 ino_t;

/// 'Cursor' type representing abstract pointer to a file in fs tree
/// This is a struct because we might want to store more information here in the future
typedef struct {
	ino_t ino;
} KompotTreeCursor_t;

/// Represents a handle to a file opened for reading/writing
typedef u64 KompotFileHandle_t;

// TODO: Create stat structure
typedef u8 KompotStatData_t;

/// Mode for opening a file
typedef u8 KompotOpenMode_t;

// TODO: Create a real structure and not just a placeholder
/// Information about a file to be created
typedef struct {
	u16 permissions;
	u16 fiele_type;
} KompotCreateInfo_t;

/// Possible request types
typedef enum : u16 {
	/// Negotiate version and max request size, sanity check for fs server
	KOMPOT_REQUEST_VERSION,
	/// Requset access to the root of fs tree handled by server
	KOMPOT_REQUEST_CONNECT,
	/// Walk the tree (move the cursor around)
	KOMPOT_REQUEST_WALK,

	/// Request information about file pointed by the cursor
	KOMPOT_REQUEST_STAT,
	KOMPOT_REQUEST_CREATE,
	KOMPOT_REQUEST_DELETE,

	/// Open a file for reading
	KOMPOT_REQUEST_OPEN,
	KOMPOT_REQUEST_READ,
	KOMPOT_REQUEST_WRITE,
	KOMPOT_REQUEST_WSTAT,
	KOMPOT_REQUEST_CLOSE,
} KompotRequestType_t;

/// Possible response types
typedef enum : u16 {
	/// Response with version and max response size, sanity check
	KOMPOT_RESPONSE_VERSION,
	/// Response with a cursor pointing to the root of the tree
	KOMPOT_RESPONSE_CONNECT,
	/// Response with a cursor at a new position (and information on how 'walks' were successful)
	KOMPOT_RESPONSE_WALK,

	/// Response with file information
	KOMPOT_RESPONSE_STAT,
	/// Response with cursor pointing to a newly created file
	KOMPOT_RESPONSE_CREATE,
	/// Empty response (no error occured)
	KOMPOT_RESPONSE_DELETE,

	/// Response with a reading handle
	KOMPOT_RESPONSE_OPEN,
	KOMPOT_RESPONSE_READ,
	KOMPOT_RESPONSE_WRITE,
	KOMPOT_RESPONSE_WSTAT,
	/// Empty response (no error occured)
	KOMPOT_RESPONSE_CLOSE,

	/// Error occured when handling a request
	KOMPOT_RESPONSE_ERROR,
} KompotResponseType_t;

/// Metadata for Kompot messages.
/// Using analogy to 'computer networks' this data lives on a *layer* above the requests.
/// Maybe because of that it should be moved to a different file...
typedef struct {
	u32 size;
	KompotMessageTag_t tag;
	union {
		KompotRequestType_t request_type;
		KompotResponseType_t response_type;
	};
} KompotMessageMetadata_t;

typedef struct {
	u32 max_size;
	KompotVersion_t version;
} KompotRequestVersion_t;

typedef struct {
	u32 max_size;
	KompotVersion_t version;
} KompotResponseVersion_t;

typedef struct {
	uid_t uid;
} KompotRequestConnect_t;

typedef struct {
	KompotTreeCursor_t cursor;
} KompotResponseConnect_t;

typedef struct {
	KompotTreeCursor_t cursor;
	uid_t uid;
	u16 walk_count;
	/// Sequence of walks to take.
	pstring_t walk_path[0]; // Of length 'path_count'
} KompotRequestWalk_t;

typedef struct {
	KompotTreeCursor_t cursor;
} KompotResponseWalk_t;

typedef struct {
	KompotTreeCursor_t cursor;
	uid_t uid;
} KompotRequestStat_t;

typedef struct {
	KompotStatData_t stat;
} KompotResponseStat_t;

typedef struct {
	KompotTreeCursor_t cursor;
	uid_t uid;
	KompotCreateInfo_t create_info;
	pstring_t name;
} KompotRequestCreate_t;

typedef struct {
	KompotTreeCursor_t cursor;
} KompotResponseCreate_t;

typedef struct {
	KompotTreeCursor_t cursor;
	uid_t uid;
} KompotRequestDelete_t;

typedef struct {
} KompotResponseDelete_t;

typedef struct {
	KompotTreeCursor_t cursor;
	uid_t uid;
	KompotOpenMode_t mode;
	// TODO: Decide what to do with 24bits left
} KompotRequestOpen_t;

typedef struct {
	KompotFileHandle_t file_handle;
	/// Max data that can be read/written in one operation (one protocol message)
	u32 max_atomic_op;
} KompotResponseOpen_t;

typedef struct {
	KompotFileHandle_t file_handle;
	u64 offset;
	u32 count;
} KompotRequestRead_t;

typedef struct {
	u32 count;
	u8 data[0];
} KompotResponseRead_t;

typedef struct {
	KompotFileHandle_t file_handle;
	u64 offset;
	u32 count;
	u8 data[0];
} KompotRequestWrite_t;

typedef struct {
	u32 count;
} KompotResponseWrite_t;

typedef struct {
	KompotFileHandle_t file_handle;
	KompotStatData_t stat_data;
} KompotRequestWstat_t;

typedef struct {
} KompotResponseWstat_t;

typedef struct {
	KompotFileHandle_t file_handle;
} KompotRequestClose_t;

typedef struct {
} KompotResponseClose_t;

typedef struct {
	pstring_t error_name;
} KompotResponseError_t;

#endif
