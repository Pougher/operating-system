#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>
#include <stdbool.h>

#include "format.h"
#include "mem.h"
#include "util.h"

// the length of the elf32 header in bytes
#define ELF32_HEADER_LENGTH 52

// the magic number is found at the start of all valid elf files
#define ELF32_MAGIC_NUMBER 0x464c457f

// value that indicates that an ELF file is little endian
#define ELF32_LITTLE_ENDIAN 0x01

// value that indicates whether an ELF file is 32 bit or 64 bit
#define ELF32_32BIT 0x01

// value that indicates the current and latest version of ELF
#define ELF32_CURRENT_VERSION 0x01

// value that indicates whether or not a file is executable
#define ELF32_EXECUTABLE 0x02

// value that indicates that the architecture of the machine the elf file is
// to be run on is x86
#define ELF32_X86 0x03

typedef struct {
    // the magic number is made up of 0x7f followed by 0x45, 0x4c and 0x46
    uint32_t magic;

    // set to either 1 or 2 to signify a 32 bit or 64 bit format
    uint8_t class;

    // byte is either set to 1 or 2 to signify little or big endianness
    uint8_t endianness;

    // set to 1 for the original and current version of ELF
    uint8_t version;

    // specifies the application binary interface (ABI) of the target operating
    // system, but since this operating system is completely different from any
    // existing ones, only the System-V ABI will be supported
    uint8_t abi;

    // the version of the ABI being targetted
    uint8_t abi_version;

    // 7 bytes of padding
    uint32_t padding_1;
    uint16_t padding_2;
    uint8_t padding_3;

    // identifies what type of object file this is
    uint16_t file_type;

    // specifies the target architecture of the system (eg. powerpc, x86, etc.)
    uint16_t architecture;

    // version, set to 1 for original elf
    uint32_t e_version;

    // the memory address of the starting point of the application
    uint32_t entry;

    // points to the program header offset
    uint32_t program_header_offset;

    // points to the segment header offset
    uint32_t section_header_offset;

    // interpretation differs by platform, probably unused in this elf
    // implementation
    uint32_t flags;

    // contains the size of the ELF header in bytes
    uint16_t header_size;

    // contains the size of a program header in bytes
    uint16_t program_header_size;

    // contains the number of entries in the program header table
    uint16_t program_header_num;

    // contains the size of a section header table entry in bytes
    uint16_t section_header_size;

    // contains the number of entries in the section table
    uint16_t section_header_num;

    // contains the index of the section header table entry that contains the
    // section names
    uint16_t section_header_index;
} __attribute__((packed)) ELFHeader;

typedef struct {
    uint32_t stub;
} ELFProgramHeader;

typedef struct {
    uint32_t stub;
} ELFSectionHeader;

typedef struct {
    // the header of the ELF file, contains information about the file in
    // general
    ELFHeader header;

    // a list of program headers, dynamically allocated as an ELF file has an
    // unknown amount of program headers at compile time
    ELFProgramHeader *program_headers;

    // a list of section headers, also dynamically allocated as an ELF file can
    // have many section headers
    ELFSectionHeader *section_headers;
} ELF32;

// checks if a given ELF file can be loaded by the ELF parser and used by the
// operating system
bool elf_check_supported(ELF32*);

// parses an ELF format file into an elf header struct, returns -1 if there
// was an error when reading the ELF file
uint32_t elf_read(char*, ELF32*);

// frees memory that is allocated to an ELF32 structure
void elf_close(ELF32*);

#endif
