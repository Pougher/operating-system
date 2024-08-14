#ifndef CORE_KENREL_FILE_SYSTEM_H
#define CORE_KERNEL_FILE_SYSTEM_H

#include "mb_module.h"
#include "multiboot.h"
#include "util.h"
#include "mem.h"
#include "file.h"
#include "allocator.h"

#include "../kernel/kinfo.h"

// the module index of the kernel-level file system
#define KERNFS_ROOT_INDEX 0

// the supported version of kernfs
#define KERNFS_VERSION 0

// maximum filename length
#define KERNFS_FILENAME_LENGTH 32

// structure that represents a single physical file in the kernel filesystem
typedef struct {
    // the name of the file
    char filename[KERNFS_FILENAME_LENGTH];

    // length of the file in bytes
    uint32_t file_length;

    // a character array of the file contents
    char *contents;
} KernfsFile;

// structure that represents the file header (present before the contents of
// every file)
typedef struct KernfsFileHeader {
    // the magic string that is present before every valid file
    char magic[4];

    // the length of the file in bytes
    uint32_t length;

    // the file's name
    char filename[KERNFS_FILENAME_LENGTH];

    // pointer to the next file header in the sequence, and is equal to NULL
    // when no more files are present after the following one
    struct KernfsFileHeader *next;
} __attribute__((packed)) KernfsFileHeader;

// structure that represents the entire kernel filesystem along with additional
// metadata that is needed to traverse the filesystem
typedef struct {
    // pointer to the root of the kernel filesystem
    char *root;

    // size of the root in bytes
    uint32_t root_size;

    // the number of files in the root
    uint32_t num_files;

    // an array of all of the files in the filesystem
    KernfsFile *files;
} Kernfs;

// structure that represents the header of the filesystem (a small section of
// information that indicates how the filesystem is organized)
typedef struct {
    // there are 6 characters at the start of the header that make up the string
    // 'KERNFS'
    char kernfs_magic_bytes[6];

    // the version number of the kernel filesystem
    uint16_t version;

    // the number of files contained in the root
    uint32_t num_files;
} __attribute__((packed)) KernfsHeader;

// initializes the kernel filesystem by loading a GRUB module as the filesystem
// root and pulling files from it
void kernfs_init(multiboot_module_t*);

// checks if a given kernel filesystem header is valid
void kernfs_check_header(KernfsHeader*);

// parses the entire kernel filesystem structure
void kernfs_parse();

// returns a pointer to a kernel file by searching the currently loaded files
// for a filename using a linear search. If no file is found, then NULL is
// returned
KernfsFile *kernfs_find_file(char*);

// opens a kernel filesystem file as a regular (albeit read only) file
File *kernfs_open(char*);

#endif
