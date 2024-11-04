#ifndef CORE_ELF_H
#define CORE_ELF_H

#include <stdint.h>
#include <stdbool.h>

#include "format.h"
#include "mem.h"
#include "util.h"
#include "allocator.h"

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
/*
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
} __attribute__((packed)) ELFHeader;*/

typedef struct {
    uint32_t magic;
    uint8_t class;
    uint8_t endianness;
    uint8_t version;
    uint8_t abi;
    uint32_t padding1;
    uint32_t padding2;
    uint16_t file_type;
    uint16_t architecture;
    uint32_t e_version;
    uint32_t entry;
    uint32_t program_header_offset;
    uint32_t section_header_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_header_size;
    uint16_t program_header_num;
    uint16_t section_header_size;
    uint16_t section_header_num;
    uint16_t section_header_index;
} __attribute__((packed)) ELFHeader;

// elf program header types
#define ELF_PT_NULL         ((uint32_t)0x00000000)
#define ELF_PT_LOAD         ((uint32_t)0x00000001)
#define ELF_PT_DYNAMIC      ((uint32_t)0x00000002)
#define ELF_PT_INTERP       ((uint32_t)0x00000003)
#define ELF_PT_NOTE         ((uint32_t)0x00000004)
#define ELF_PT_SHLIB        ((uint32_t)0x00000005)
#define ELF_PT_PHDR         ((uint32_t)0x00000006)
#define ELF_PT_TLS          ((uint32_t)0x00000007)
#define ELF_PT_LOOS         ((uint32_t)0x60000000)
#define ELF_PT_HIOS         ((uint32_t)0x6FFFFFFF)
#define ELF_PT_LOPROC       ((uint32_t)0x70000000)
#define ELF_PT_HIPROC       ((uint32_t)0x7FFFFFFF)

typedef struct {
    // identifier for the type of segment
    uint32_t p_type;

    // offset of the segment in the file image
    uint32_t p_offset;

    // virtual address of the segment in memory
    uint32_t p_vaddr;

    // on systems where physical addresses are relevant, reserved for the
    // segment's physical address
    uint32_t p_addr;

    // size in bytes of the segment in the file image
    uint32_t p_filesz;

    // size in bytes of the segment in memory
    uint32_t p_memsz;

    // segment dependent flags
    uint32_t p_flags;

    // the alignemnt of the program header
    uint32_t p_align;
} __attribute__((packed)) ELFProgramHeader;

// elf section header types
#define ELF_SHT_NULL        ((uint32_t)0x00000000)
#define ELF_SHT_PROGBITS    ((uint32_t)0x00000001)
#define ELF_SHT_SYMTAB      ((uint32_t)0x00000002)
#define ELF_SHT_STRTAB      ((uint32_t)0x00000003)
#define ELF_SHT_RELA        ((uint32_t)0x00000004)
#define ELF_SHT_HASH        ((uint32_t)0x00000005)
#define ELF_SHT_DYNAMIC     ((uint32_t)0x00000006)
#define ELF_SHT_NOTE        ((uint32_t)0x00000007)
#define ELF_SHT_NOBITS      ((uint32_t)0x00000008)
#define ELF_SHT_REL         ((uint32_t)0x00000009)
#define ELF_SHT_SHLIB       ((uint32_t)0x0000000A)
#define ELF_SHT_DYNSYM      ((uint32_t)0x0000000B)
#define ELF_SHT_INIT_ARRAY  ((uint32_t)0x0000000E)
#define ELF_SHT_FINI_ARRAY  ((uint32_t)0x0000000F)
#define ELF_SHT_PREI_ARRAY  ((uint32_t)0x00000010)
#define ELF_SHT_GROUP       ((uint32_t)0x00000011)
#define ELF_SHT_SYMTAB_SHDX ((uint32_t)0x00000012)
#define ELF_SHT_NUM         ((uint32_t)0x00000013)
#define ELF_SHT_LOOS        ((uint32_t)0x60000000)
#define ELF_SHT_HIOS        ((uint32_t)0x6FFFFFFF)
#define ELF_SHT_LOPROC      ((uint32_t)0x70000000)
#define ELF_SHT_HIPROC      ((uint32_t)0x7FFFFFFF)
#define ELF_SHT_LOUSER      ((uint32_t)0x80000000)
#define ELF_SHT_HIUSER      ((uint32_t)0x8FFFFFFF)

// elf section header attributes
#define ELF_SHF_WRITE       ((uint32_t)0x00000001)
#define ELF_SHF_ALLOC       ((uint32_t)0x00000002)
#define ELF_SHF_EXECINSTR   ((uint32_t)0x00000004)
#define ELF_SHF_MERGE       ((uint32_t)0x00000010)
#define ELF_SHF_STRINGS     ((uint32_t)0x00000020)
#define ELF_SHF_INFO_LINK   ((uint32_t)0x00000040)
#define ELF_SHF_LINK_ORDER  ((uint32_t)0x00000080)
#define ELF_SHF_OS_NONCONF  ((uint32_t)0x00000100)
#define ELF_SHF_GROUP       ((uint32_t)0x00000200)
#define ELF_SHF_TLS         ((uint32_t)0x00000400)
#define ELF_SHF_MASKOS      ((uint32_t)0x0FF00000)
#define ELF_SHF_MASKPROC    ((uint32_t)0xF0000000)
#define ELF_SHF_ORDERED     ((uint32_t)0x04000000)
#define ELF_SHF_EXCLUDE     ((uint32_t)0x08000000)

typedef struct {
    // an offset to a string in the .shstrtab section that represents the name
    // of this header
    uint32_t sh_name;

    // identifies the type of the header
    uint32_t sh_type;

    // section header flags
    uint32_t sh_flags;

    // virtual address of the section in memory, for sections that are loaded
    uint32_t sh_addr;

    // offset of the section in the file image
    uint32_t sh_offset;

    // size of the section in bytes
    uint32_t sh_size;

    // the section header table index link
    uint32_t sh_link;

    // contains the section index of an associated section. This field is used
    // for several purposes, depending on the type of section
    uint32_t sh_info;

    // contains the required alignment of the section
    uint32_t sh_addralign;

    // contains the size in bytes of each entry for sections that contains
    // fixed-size entries. Otherwise this field contains zero
    uint32_t sh_entsize;
} __attribute__((packed)) ELFSectionHeader;

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
void elf_read(char*, ELF32*);

// loads the section and program headers into buffers using the character
// pointer array as the elf file. Dats is allocated on the heap by this function
// and so the headers must be free'd after use
void elf_load_headers(char*, ELF32*);

// frees memory that is allocated to an ELF32 structure
void elf_close(ELF32*);

#endif
