#include "kernfs.h"

static Kernfs kernfs = { 0 };

void kernfs_init(multiboot_module_t *fs) {
    kernfs.root = (char*)(
        (uint32_t)fs->mod_start + (uint32_t)KERNEL_VIRTUAL_OFFSET
    );
    kernfs.root_size = fs->mod_end - fs->mod_start;

    KernfsHeader *header = (KernfsHeader*)kernfs.root;
    kernfs_check_header(header);

    // allocate enough space for all of the file structs
    kernfs.num_files = header->num_files;
    kernfs.files = kmalloc(sizeof(KernfsFile) * kernfs.num_files);

    if (kernfs.files == NULL) {
        kpanic("Could not allocate kernfs file list");
    }

    // parse the filesystem, as all of the required information has been
    // extracted from the file system header
    kernfs_parse();
}

void kernfs_check_header(KernfsHeader *header) {
    // first, check if the magic bytes are correct
    if (strncmp(header->kernfs_magic_bytes, "KERNFS", 6) != 0) {
        kpanic("Kernel Level Filesystem has an incorrect header");
    }

    // check if the kernel version stated in the header is equal to the
    // supported version
    if (header->version != KERNFS_VERSION) {
        kpanic("Kernel Filesystem header has an incorrect version number");
    }
}

void kernfs_parse() {
    if (kernfs.num_files == 0) return;

    char *files_base = (char*)kernfs.root + sizeof(KernfsHeader);
    KernfsFileHeader *current_file = (KernfsFileHeader*)files_base;
    uint32_t file_index = 0;

    while (true) {
        if (strncmp(current_file->magic, "file", 4) != 0) {
            kpanic("Invalid file magic number detected");
        }

        KernfsFile *file = &kernfs.files[file_index];
        memcpy(file->filename, current_file->filename, KERNFS_FILENAME_LENGTH);
        file->file_length = current_file->length;
        file->contents = (char*)current_file + sizeof(KernfsFileHeader);

        if (current_file->next == 0) {
            return;
        }
        current_file = (KernfsFileHeader*)(
            (uint32_t)current_file->next + (uint32_t)kernfs.root
        );

        file_index++;
    }
}

KernfsFile *kernfs_find_file(char *filename) {
    // simple linear search, not very efficient but is easy to implement. I may
    // use a different searching algorithm later, but for now this section of
    // code is not performance critical
    for (uint32_t i = 0; i < kernfs.num_files; i++) {
        if (strcmp(kernfs.files[i].filename, filename) == 0) {
            return &kernfs.files[i];
        }
    }

    // unfortunately, no file with a matching filename was found
    return NULL;
}

// this read function is run whenever a file read is performed on a kernel
// file system file
static void kernfs_read_function(File *file, void *object, uint32_t length) {
    if (file->file_pointer == 0) {
        return;
    }

    // cap the number of bytes that can be read in the case that length is
    // larger than the number of bytes available
    if (file->file_pointer < length) {
        length = file->file_pointer;
    }

    memcpy(object, file->state, length);
    memcpy(
        file->state,
        (char*)file->state + length,
        file->file_pointer - length
    );

    file->file_pointer -= length;
}

// the kernel file system file close function simply frees the memory allocated
// to the file buffer
static void kernfs_close_function(File *file) {
    kfree(file->state);
}

File *kernfs_open(char *filename) {
    KernfsFile *kern_file = kernfs_find_file(filename);

    // open the kernfs file as a File* object
    uint32_t file_desc = file_create(
        NULL,
        NULL,
        kernfs_read_function,
        kernfs_close_function
    );
    File *file = file_descriptor_to_pointer(file_desc);

    file->state = kmalloc(kern_file->file_length);
    if (!file->state) {
        kpanic("Could not allocate enough space for kernfs file");
    }

    file->file_pointer = kern_file->file_length;
    memcpy(file->state, kern_file->contents, kern_file->file_length);

    return file;
}
