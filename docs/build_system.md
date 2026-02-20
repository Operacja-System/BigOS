# CMake Organization and Usage Guide

## Overview

BigOS uses CMake (version 3.24+) as its primary build system to compile a RISC-V 64-bit operating system kernel and supporting examples. The build system is organized to support multiple compiler toolchains (GCC and Clang), various build configurations, and automated binary generation and testing.

## Directory Structure

```
BigOS/
├── CMakeLists.txt              # Root CMake configuration
├── CMakePresets.json           # Build presets for easy configuration
├── CMake/                      # CMake modules and configuration
│   ├── CommonConfig.cmake      # Shared build functions and configuration
│   ├── Linters.cmake          # Linter integration (clang-tidy, include-what-you-use)
│   ├── preset-mixins.json     # Preset configuration mixins
│   └── toolchains/            # Cross-compilation toolchain definitions
│       ├── riscv64-common.cmake     # Common RISC-V settings
│       ├── riscv64-gcc.cmake        # GCC-specific toolchain
│       └── riscv64-clang.cmake      # Clang-specific toolchain
├── src/                        # Source code for all components
│   ├── CMakeLists.txt
│   ├── kernel/                # OS kernel
│   └── lib/                   # Libraries
└── build/                      # Build output directory
```

## Build Configurations

### CMakePresets.json

Provides pre-configured build profiles for easy project setup:

Some notable examples:
- **debug**: GCC with debug symbols
- **release**: GCC with optimization

- **debug-clang**: Clang with debug symbols and code analysis
- **release-clang**: Clang with optimization

- **ninja-multi**: Multi-configuration Ninja build with GCC (see below)
- **ninja-multi-clang**: Multi-configuration Ninja build with Clang

- **release-lto**: GCC with optimization and link-time optimization
- **release-lto-clang**: Clang with optimization and link-time optimization
- **ninja-multi-lto**: Multi-configuration Ninja build with GCC and link-time optimization
- **ninja-multi-lto-clang**: Multi-configuration Ninja build with Clang and link-time optimization

### Using Presets

Configure with a preset:
```bash
cmake --preset debug
```

Then build:
```bash
cmake --build --preset debug
```

Or configure manually:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=CMake/toolchains/riscv64-gcc.cmake \
      -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---
Note:
If you pull in a new patch that possibly changes:
- CMakePresets.json
- CMake/preset-mixins.json
- any file in CMake/toolchains

You NEED to cleanup the build directory:
```sh
rm -rf build
cmake --preset "<my-preset>"
```
or
```sh
cmake --fresh --preset "<my-preset>"
```

Please check if this before submitting any issues.

### Build Targets

To build every binary just run:
```sh
cmake --build build  # -j for parallel builds
```

The output files should be in specific subdirectory of ./build
following the project directory structure.

All libraries and executables have their own targets for building.
Additionally some binaries have an additional run-* and debug targets.

Those can be run with the following commands:
```sh
cmake --build build -t run-<target-name>
cmake --build build -t debug-<target-name>
```

Tab-completion should help you find the target names.

### Generators

You can additionally specify a generator.
By default, CMake uses Unix Makefiles on linux.
You can also use Ninja for faster builds.

```sh
cmake --preset debug --generator Ninja
cmake --build --preset debug --config Debug
```

or you can use ninja-multi generator to have all available build types at once:
```sh
cmake --preset ninja-multi
cmake --build build --config Debug 
cmake --build build --config Release
cmake --build build --config RelWithDebInfo
```

Please take note that parallel builds are supported with all generators, but Ninja has it by default.
For other generators use:
```sh
cmake --build build -j
```

### Linters

Integrates static analysis tools into the build:

- `BIGOS_USE_CLANG_TIDY`: Enable clang-tidy code analysis (default: OFF)
- `BIGOS_USE_INCLUDE_WHAT_YOU_USE`: Check for unnecessary includes (default: OFF)

When enabled, these tools run automatically during compilation.

You can enable those by providing them as CMake options:
```sh
cmake -DBIGOS_USE_CLANG_TIDY=ON -DBIGOS_USE_INCLUDE_WHAT_YOU_USE=ON --preset="<preset-name>"
```

### QEMU Settings

To modify QEMU settings you can use the following command:
```bash
cmake -B build \
  -DBIGOS_QEMU_OPTIONS="-machine virt -serial mon:stdio -nographic -m 256M"
```

### Toolchain Files

Cross-compilation for RISC-V 64-bit targets requires specialized toolchain configuration.

**riscv64-common.cmake**
- Sets cross-compilation target to RISC-V 64
- Defines RISC-V architecture: `rv64ima_zicsr_zifencei`
- Defines ABI: `lp64`
- Defines code model: `medany`
- Sets common compiler flags: `-ffreestanding` `-nostdlib`

**riscv64-gcc.cmake**
- Includes common RISC-V configuration
- Auto-detects GCC toolchain prefix
- Supports multiple prefix formats: `riscv64-unknown-linux-elf`, `riscv64-linux-gnu`, etc.

**riscv64-clang.cmake**
- Similar to GCC variant but for Clang compiler

### Other options

To list all available options see:
```sh
cmake -LH build
```

To set a variable just run:
```sh
cmake -D<variable>=<value> build
```

Please take note that all ABI-breaking changes should be made on a clean build directory
and not as a modification:
```sh
cmake --preset="<preset_name>" -DVAR1=value1 -DVAR2=value2 --fresh
```

If you don't know if an option is ABI-breaking, err on the side of caution.

## CMake Components

The project supplies some helper functions and macros:

**SETUP_LIBRARY(name)**
- Initializes a library target
- Automatically collects all `.c` and `.S`/`.s` assembly files
- Collects headers from `include/<name>/` directory
- Applies common configuration

**SETUP_EXECUTABLE(name)**
- Initializes an executable target
- Automatically collects all `.c`, `.h`, and `.S`/`.s` files
- Applies common configuration

**COMPILE_BINARY(name)**
- Converts executable to raw binary format (`.bin`)
- Uses `objcopy` to strip ELF headers
- Useful for bootloaders and kernels

**ADD_QEMU_TARGET(name)**
- Creates run targets for QEMU emulation
- Creates `run-<name>` target to execute the binary
- Creates `debug-<name>` target with GDB debugging support (`-S -s`)
- Optional `BIOS_IMAGE` parameter to use as BIOS instead of kernel

### Library Example: newlib

- in a subdirectory of src/lib/ create 'newlib'
- add CMakeLists.txt to the directory
- optionally add `include/newlib/` directory for headers

CMakeLists.txt
```cmake
SETUP_LIBRARY(newlib)
```

This automatically setups everything you need.

The sources are in the same directory as CMakeLists.txt.
The public headers are in include/newlib/ and private are with the sources.

After that you can add any customizations needed, e.g:
- dependencies:
```cmake
target_link_libraries(newlib
            PUBLIC lib1 lib2 ...  # for libraries that should be traisitively added 
            PRIVATE lib3 lib4 ...  # for libraries for internal use (not visible in public headers)
)
```

### Executable Example: newbin

- in a subdirectory of src/ create 'newbin'
- add CMakeLists.txt to the directory

```cmake
SETUP_EXECUTABLE(newbin)
target_link_libraries(newbin PRIVATE newlib)  # same as above
COMPILE_BINARY(example_sbi)  # creates a raw binary for consumption by qemu
ADD_QEMU_TARGET(example_sbi)  # add run-newbin and debug-newbin targets
```

### Output Files

Built binaries are located in:
```
build/src/<component>/
├── <name>          # ELF executable
└── <name>.bin      # Raw binary (bootable)
```

## Project Layout Best Practices

- **Libraries**: Put reusable code in `src/lib/` with headers in `include/`
- **Executables**: Put in `src/`
- **Headers**: Public headers go in `include/<component>/`, private in source directory
