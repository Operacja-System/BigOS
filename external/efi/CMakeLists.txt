project(gnu-efi)

add_library(gnu-efi INTERFACE)

set(EFI_DIR ${PROJECT_SOURCE_DIR})

set(LINKER_SCRIPT ${EFI_DIR}/lib/elf_riscv64_efi.lds)

target_link_options(gnu-efi INTERFACE
	-T ${LINKER_SCRIPT} ${EFI_DIR}/lib/crt0-efi-riscv64.o
	LINKER:-nostdlib,-znocombreloc,-Bsymbolic)

target_compile_options(gnu-efi INTERFACE
	-fPIC -ffreestanding
	 -fno-stack-protector -fno-stack-check
	 -fshort-wchar -mabi=lp64d -march=rv64g
)

target_include_directories(gnu-efi SYSTEM INTERFACE ${EFI_DIR}/include/efi)
target_link_directories(gnu-efi INTERFACE ${EFI_DIR}/lib)
target_link_libraries(gnu-efi INTERFACE efi gnuefi)
