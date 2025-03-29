# Showcase

> This is here only to showcase the theme.

## H2 BigOS

### H3 BigOS

#### H4 BigOS

##### H5 BigOS

Operating system made by students of the University of Wroclaw.  
Lorem ipsum dolor sit amet, **consectetur** adipiscing elit, *sed do eiusmod* tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.

Some c code:

```c
int main() {
  printf("Hello there");
}
```

Cmake build script:

```cmake
cmake_minimum_required(VERSION 3.24)

find_program(QEMU_FOUND_PATH "qemu-system-riscv64")

set(BIGOS_WARNINGS_AS_ERRORS OFF CACHE BOOL "Treat warnings as errors")
set(BIGOS_QEMU_PATH "${QEMU_FOUND_PATH}" CACHE PATH "path to qemu")
set(BIGOS_QEMU_OPTIONS "-machine virt -serial mon:stdio -nographic" CACHE STRING "options for qemu")
```

Risc-v asm:

```riscv
.section .text
.global strlen
strlen:
    # a0 = const char *str
    li     t0, 0         # i = 0
1: # Start of for loop
    add    t1, t0, a0    # Add the byte offset for str[i]
    lb     t1, 0(t1)     # Dereference str[i]
    beqz   t1, 1f        # if str[i] == 0, break for loop
    addi   t0, t0, 1     # Add 1 to our iterator
    j      1b            # Jump back to condition (1 backwards)
1: # End of for loop
    mv     a0, t0        # Move t0 into a0 to return
    ret                  # Return back via the return address register
```

Bash script:

```bash
baseCommit=$1
outputFile=$2
url=$3

if test "$#" -ne 1 && test "$#" -ne 3 || test -z "$1"
then
	echo "USAGE: $0 <BASE_COMMIT> [<OUTPUT_FILE> <URL>]"
	exit 1
fi
```

---

Some very normal text

> Some quote or whatever.

- **Bold Fragment**
- *Some Italics*
- [Link to a header](#h2-bigos)
- [link to another page](../About)
    - And a nested bullet point

