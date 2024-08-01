#include "file.h"

FileDescriptor stdout   = FILE_INVALID;
FileDescriptor stdin    = FILE_INVALID;
FileDescriptor stderr   = FILE_INVALID;

// list of files that can be indexed into by a file descriptor
File files[MAXIMUM_FILES] = { 0 };

void file_init() {
    stdout = file_create(stdout_write);
}

int32_t file_create(void (*write_function)(File*, char*, uint32_t)) {
    // firstly, search for an unopened file
    File *file = NULL;
    uint16_t i;

    for (i = 0; i < MAXIMUM_FILES; i++) {
        if (!files[i].is_open) {
            // an unopened file has been found
            file = &files[i];
            break;
        }
    }

    // check if an open file has been found
    if (file == NULL) {
        return FILE_INVALID;
    }

    // since we have found an unopened file, we can go ahead and declare the
    // file as opened with the parameters specified
    file_declare(file, write_function);

    return (int32_t)i;
}

int32_t file_delete(FileDescriptor descriptor) {
    if (descriptor >= MAXIMUM_FILES) {
        return FILE_INVALID;
    }

    files[descriptor].is_open = false;
    return FILE_SUCCESS;
}

void file_declare(File *fp, void (*write_function)(File*, char*, uint32_t)) {
    fp->file_pointer = 0;
    fp->write_function = write_function;
    fp->is_open = true;
}

int32_t file_write(FileDescriptor descriptor, char *bytes, uint32_t length) {
    if (descriptor >= MAXIMUM_FILES) {
        return FILE_INVALID;
    }

    File *file = &files[descriptor];

    if (!file->is_open) {
        return FILE_INVALID;
    }

    file->file_pointer += length;
    file->write_function(file, bytes, length);

    return FILE_SUCCESS;
}
