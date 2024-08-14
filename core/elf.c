#include "elf.h"

bool elf_check_supported(ELF32 *elf) {
    return (elf->header.endianness == ELF32_LITTLE_ENDIAN) &&
        (elf->header.class == ELF32_32BIT) &&
        (elf->header.version == ELF32_CURRENT_VERSION) &&
        (elf->header.file_type == ELF32_EXECUTABLE) &&
        (elf->header.architecture == ELF32_X86);
}

void elf_read(char *data, ELF32 *elf_file) {
    // copy the first 52 bytes from the elf file into the elf header
    memcpy(&(elf_file->header), data, ELF32_HEADER_LENGTH);

    if (elf_file->header.magic != ELF32_MAGIC_NUMBER) {
        kpanic("Attempted to load an ELF file with an invalid magic number");
        return;
    }

    if (!elf_check_supported(elf_file)) {
        kpanic("Attempted to load an unsupported ELF file");
        return;
    }

    // allocate enough memory for all the ELF headers
}

void elf_close(ELF32 *elf) {
    kfree(elf->program_headers);
    kfree(elf->section_headers);
}
