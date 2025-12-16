#ifndef _FILE_TABLE_H_
#define _FILE_TABLE_H_

#include "vfs.h"

/// Adds an entry to the users FileTable.
// TODO: Implement. No user space so this is mocked
FtEntry_t* ft_add_entry();

/// Remove entry from users FileTable.
// TODO: Implement. No user space so this is mocked
void ft_free_entry(FtEntry_t* entry);

#endif
