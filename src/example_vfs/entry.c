#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

#include "../kernel/vfs/vfs.h"

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	vfsmain();
}
