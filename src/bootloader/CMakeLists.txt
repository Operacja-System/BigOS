set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

add_library(bootloader SHARED)
SETUP_COMMON(bootloader)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.h)
file(GLOB_RECURSE AS_SOURCES CONFIGURE_DEPENDS *.s)

target_sources(bootloader
  PRIVATE
    ${SOURCES}
    ${AS_SOURCES}
  PRIVATE
    FILE_SET HEADERS
    BASE_DIRS .
    FILES ${HEADERS}
  )

target_link_libraries(bootloader gnu-efi)

set(BOOTLOADER_EFI_LOCATION "$<TARGET_FILE_DIR:bootloader>/$<TARGET_FILE_BASE_NAME:bootloader>.efi")

add_custom_command(
    TARGET bootloader POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
                -j .text -j .sdata -j .data -j .rodata
                -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.*
                -j .rela.* -j .reloc --target efi-app-riscv64 --subsystem=10
                "$<TARGET_FILE:bootloader>"
                "${BOOTLOADER_EFI_LOCATION}"
    VERBATIM
)

set(EFI_DIR "${CMAKE_BINARY_DIR}/efidir")
set(EFI_BOOT_DIR "${EFI_DIR}/EFI/BOOT")
set(BOOTRISCV64_EFI "${EFI_BOOT_DIR}/BOOTRISCV64.EFI")
set(EXT2_BINARY_SRC "${PROJECT_SOURCE_DIR}/external/ext2efi/ext2_riscv64.efi")
set(EXT2_BINARY_EFI "${EFI_BOOT_DIR}/ext2.efi")

file(MAKE_DIRECTORY "${EFI_BOOT_DIR}")

add_custom_command(
    OUTPUT ${BOOTRISCV64_EFI}
    COMMAND ${CMAKE_COMMAND} -E copy "${BOOTLOADER_EFI_LOCATION}" "${BOOTRISCV64_EFI}"
    DEPENDS bootloader
    COMMENT "Copying bootloader EFI to ${BOOTRISCV64_EFI}"
    VERBATIM
)

add_custom_command(
    OUTPUT ${EXT2_BINARY_EFI}
    COMMAND ${CMAKE_COMMAND} -E copy "${EXT2_BINARY_SRC}" "${EXT2_BINARY_EFI}"
    DEPENDS "${EXT2_BINARY_SRC}"
    COMMENT "Copying ext2 EFI driver to ${EXT2_BINARY_EFI}"
    VERBATIM
)

add_custom_target(
	efi-partition-structure ALL
	DEPENDS ${BOOTRISCV64_EFI} ${EXT2_BINARY_EFI}
)

add_custom_target(run-bootloader
	COMMAND "${BIGOS_QEMU_PATH}" ${BIGOS_QEMU_OPTIONS_LIST}
			-kernel "${PROJECT_SOURCE_DIR}/external/u-boot/u-boot.bin"
			-hda fat:rw:${EFI_DIR}
    DEPENDS efi-partition-structure
    VERBATIM
    USES_TERMINAL
)
