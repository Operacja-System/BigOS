# VFS FS Server Communication Protocoll (KOMPOT)

> For reference see `vfs/kompot.h`

This is the protocol used for communication with FS Servers.
It allows for performing all necesary operations to manage, access and modify a file system.

## FS Server

This term describes a userspace application that:

1. Creates a server handle via call to `open` in `{TBD, maybe /serv?/}/`. The result of this call will be a readable `fd`.
2. Accepts and responds to messages on `fd` using FS server protocol

## Protocol

*Kompot* consists of requests and responses to requests; all requests must receive exactly one response.

### Conventions

Binary representations for data sent over streams use the following conventions:

- All multi-byte integers are sent in **little**-endian format
- Fields are packed tightly, no implicit padding is used
- Fields are listed in the order they are sent
- `data[<size>]` corresponds to a `<size>` byte field named `data`
  - If `<size>` is not a constant then the length is specified by a preceding field
- `(...)[<count>]` corresponds to an array of `<count>` elements
  - If `<count>` is not a constant then the length is specified by a preceding field
- Strings are sent in UTF-8 encoding **without** a trailing null byte

### Metadata

All requests/responses preceded by a **metadata** section containing:

```text
size[4]    tag[2]    type[2]
```

Where:

- `size` – total size of the request (including this field)
- `tag` – **unique** identifier for the request
- `type` – type of request/response; Fs servers receive requests type, vfs receives response type

> This can be thought of as a transport layer header for the protocol.

### Request-Response Types

#### VERSION

Used to initiate connection, exactly one request-response messages should be exchanged before any other request is made.
Client and server negotiate the maximal response size and protocol version (not used now, but maybe...).

> If error response is received the server will not be registered.

##### Request

```text
max_size[4]    version[4]
```

Where:

- `max_size` – max total response size
- `version` – client protocol version implementation

##### Response

```text
max_size[4]    version[4]
```

Where:
- `u32 max_size` – max total request size
- `u32 FSProtocolVersion_t version` – server protocol version implementation

#### [CONNECT](Walk.md)

Used to receive a **cursor** to a root of fs tree.
The **cursor** is an abstract pointer to a file used for navigating the fs tree.

##### Request

```text
uid[4]
```

Where:

- `uid` – user ID for checking permissions


##### Response

```text
cursor[8]
```

Where:

- `cursor` – a unique identifier for a file in the file system


#### [WALK](Walk.md)

Given a **cursor** and a list of names of subsequent directories to enter, this request returns a new **cursor** to the last directory in the list.

##### Request

```text
cursor[8]    uid[4]    walk_count[2]    (name_size[2]  name[name_size])[walk_count]
```

Where:

- `cursor` – a **cursor** from which to start walking the file system
- `uid` – user ID for checking permissions
- `walk_count` – number of subsequent directories to enter
  - `name_size` – size of the name in bytes
  - `name` – name of the directory to enter

##### Response

```text
cursor[8]
```

Where:

- `cursor` – a new **cursor** to the last directory in the list. If the walk was unsuccessful an error response is returned instead.

#### [STAT](Stat.md)

Given a **cursor** returns metadata of pointed file.

##### Request

```text
cursor[8]    uid[4]
```

Where:

- `cursor` – a **cursor** to the file to get metadata for
- `uid` – user ID for checking permissions

##### Response

```text
size[2] stat[size]
```

Where:

- `size` – size of the `stat` structure in bytes
- `stat` – a `struct stat` containing metadata of the file

#### [CREATE](Create_Delete.md)

Creates a new file or directory at the location pointed to by the **cursor**.

##### Request

```text
cursor[8]    uid[4]    options[4]    name_size[2]    name[name_size]
```

Where:

- `cursor` – a **cursor** to the directory to create the file in
- `uid` – user ID for checking permissions
- `options` – options for creating the file, see `CREATE_OPTIONS`
- `name_size` – size of the name in bytes
- `name` – name of the file to create

##### Response

```text
cursor[8]
```

Where:

- `cursor` – a new **cursor** to the created file. If the creation was unsuccessful an error response is returned instead.

#### [DELETE](Create_Delete.md)

Deletes a file or directory at the location pointed to by the **cursor**.
After deletion the **cursor** is no longer valid.

##### Request

```text
cursor[8]    uid[4]
```

Where:

- `cursor` – a **cursor** to the file to delete
- `uid` – user ID for checking permissions

##### Response

Empty response (consisting only of metadata) is given on success, error response is returned otherwise.

#### [OPEN](Open.md)

Given a **cursor** opens a file and returns a handle for reading/writing (depending on flags).
Returned handle is guaranteed to be unique and valid until the file is closed (unless server explodes).

##### Request

```text
cursor[8]    uid[4]    flags[4]
```

Where:

- `cursor` – a **cursor** to the file to open
- `uid` – user ID for checking permissions
- `flags` – flags for opening the file, see `OPEN_FLAGS`

##### Response

```text
handle[8]    max_atomic_op[4]
```

Where:

- `handle` – a unique identifier for the opened file, used for subsequent read/write operations. If the open was unsuccessful an error response is returned instead.
- `max_atomic_op` – maximum size of an atomic operation that can be performed on the file, in bytes.

#### [READ](Read.md)

Given a *readable* **handle** reads data from the file.

##### Request

```text
handle[8]    offset[8]    size[4]
```

Where:

- `handle` – a **handle** to the file to read from
- `offset` – offset in the file to read from, in bytes
- `size` – size of the data to read, in bytes

##### Response

```text
size[4]    data[size]
```

Where:

- `size` – size of the data read, in bytes
- `data` – data read from the file


#### [WRITE](Write.md)

Given a *writeable* **handle** writes data to the file.

##### Request

```text
handle[8]    offset[8]    size[4]    data[size]
```

Where:

- `handle` – a **handle** to the file to write to
- `offset` – offset in the file to write to, in bytes
- `size` – size of the data to write, in bytes
- `data` – data to write to the file

##### Response

```text
size[4]
```

Where:

- `size` – size of the data written, in bytes

#### [WSTAT](Stat.md)

Given a *writeable* **handle** updates metadata of the file.

##### Request

```text
handle[8]    size[2]    stat[size]
```

Where:

- `handle` – a **handle** to the file to update metadata for
- `size` – size of the `stat` structure in bytes
- `stat` – a `struct stat` containing metadata to update

##### Response

Empty response is given on success.

#### [CLOSE](Open.md)

Closes a file handle, after this call the handle is no longer valid.

##### Request

```text
handle[8]
```

Where:

- `handle` – a **handle** to the file to close

##### Response

Empty response is given on success.

#### ERROR

This is a special response sent by the server in case of an error.
It can be sent in response to any request, but is not a request itself.

##### Response

```text
size[2]    error_name[size]
```

Where:

- `size` – size of the `error_name` string in bytes
- `error_name` – name of the error, a string in UTF-8 encoding without a trailing null byte
