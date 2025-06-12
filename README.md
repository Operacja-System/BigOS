# BigOS

BigOS is an operating system developed by students at the University of Wrocław.  

## Dependencies (minimum version)

- **CMake** (3.24) + makefile generator
- **qemu-system-riscv**
- **riscv64-elf-gcc**
- **riscv64-linux-gnu-gcc**
- **clang-format** (19.0)
- **MkDocs** (1.4.2) – only for documentation

---

## Building Quickstart

Complete information about build system can be found in [build system docs](https://Operacja-System.github.io/BigOS/build_system/).  
Here is just enough to get you started.

### Cross-Compiling

Clone and enter the repository, then run:

```bash
cmake --preset=<your-preset>
cmake --build
```

To list all available presets run:

```bash
cmake --list-presets
# Some notable presets:
# debug
# release
# release-size (optimize for size)
# release-debug (release with debug info)
```

#### Running

To run the OS in QEMU use one of the targets:

```bash
cmake --build build --target run-<target to run> # ex. run-example_sbi
# To exit just enter Ctrl+a x
```

All available targets are listed in [build system target docs](build_system/targets).

---

## Contributing

Before contributing please familiarize yourself with [the rules](contributing_rules.md).

---

## Documentation

Documentation is available at [github pages](https://Operacja-System.github.io/BigOS).

### Offline Documentation

Can be generated using MkDocs:

```bash
mkdocs build
# Or to serve it locally:
# mkdocs serve
```
