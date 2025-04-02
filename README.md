# BigOS
BigOS is an operating system developed by students at the University of Wrocław.

### Dependencies (minimum version)
- **CMake** (3.24)
- **qemu-system-riscv64**
- **riscv64-elf-gcc**
- **dosfstools**
- **e2fsprogs**
- **mtools**

### Contributing
Before contributing please familiarize yourself with [the rules](Markdown/contributing_rules.md).

### Documentation
All [acronyms](Markdown/acronyms.md) used in this project.

### Building

Cross compilation:
```sh
git clone https://github.com/Operacja-System/BigOS.git
cd BigOS
cmake --preset=release
cmake --build build

# for all available presets see
cmake --list-presets

# some notable presets:
# debug
# release
# release-size (optimize for size)
# release-debug (release with debug info)
```

Then either you can install produced binaries:
```sh
cmake --install build --prefix <install dir>
```

Or you can run or debug targets
```sh
cmake --build build --target run-<target to run> # e.g. run-example_sbi
# or --target debug-

# To exit just Ctrl-A x
```

### Debugging
```sh
cmake --build build --target debug-<target>
gdb <path to elf> -ex 'target remote localhost:1234'
```

### [License](LICENSE)

