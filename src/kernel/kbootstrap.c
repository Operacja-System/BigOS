#include <stdbigos/types.h>

[[noreturn]] void kinit();

[[noreturn]] void kbootstrap(u64 load_address, u64 load_size, u64 dt_ppn) {

	kinit();
}
