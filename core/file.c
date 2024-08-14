#include "file.h"

FileDescriptor stdout   = FILE_INVALID;
FileDescriptor stdin    = FILE_INVALID;
FileDescriptor stderr   = FILE_INVALID;

// list of files that can be indexed into by a file descriptor
File files[MAXIMUM_FILES] = { 0 };

// returns NULL if the file descriptor provided is invalid, otherwise returns
// the file pointer associated with the provided file descriptor
static inline File *file_descriptor_valid(FileDescriptor descriptor) {
    if (descriptor >= MAXIMUM_FILES) {
        return NULL;
    }

    File *file = &files[descriptor];
    if (!file->is_open) {
        return NULL;
    }

    return file;
}

void file_init() {
    stdout = file_create(NULL, stdout_write, NULL, NULL);
    stdin  = file_create(stdin_init, stdin_write, stdin_read, stdin_close);
}

FileDescriptor file_create(
    FileInitFunc init_function,
    FileWriteFunc write_function,
    FileReadFunc read_function,
    FileCloseFunc close_function) {
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
    file_declare(
        file,
        init_function,
        write_function,
        read_function,
        close_function
    );

    // since the file has now been completely set up, we can call the
    // initialization function
    if (file->init_function) {
        file->init_function(file);
    }

    return (int32_t)i;
}

int32_t file_delete(FileDescriptor descriptor) {
    File *file;
    if (!(file = file_descriptor_valid(descriptor))) {
        return FILE_INVALID;
    }
    if (file->close_function) {
        file->close_function(file);
    }

    files[descriptor].is_open = false;
    return FILE_SUCCESS;
}

void file_declare(
    File *fp,
    FileInitFunc init_function,
    FileWriteFunc write_function,
    FileReadFunc read_function,
    FileCloseFunc close_function) {
    fp->file_pointer = 0;
    fp->init_function = init_function;
    fp->write_function = write_function;
    fp->read_function = read_function;
    fp->close_function = close_function;
    fp->is_open = true;
}

int32_t file_write(FileDescriptor descriptor, char *bytes, uint32_t length) {
    File *file;
    if (!(file = file_descriptor_valid(descriptor))) {
        return FILE_INVALID;
    }

    if (file->write_function) {
        file->write_function(file, bytes, length);
    }

    return FILE_SUCCESS;
}

int32_t file_read(FileDescriptor descriptor, void *object, uint32_t length) {
    File *file;
    if (!(file = file_descriptor_valid(descriptor))) {
        return FILE_INVALID;
    }

    if (file->read_function) {
        file->read_function(file, object, length);
    }

    return FILE_SUCCESS;
}

int32_t file_write_char(FileDescriptor descriptor, char c) {
    File *file;
    if (!(file = file_descriptor_valid(descriptor))) {
        return FILE_INVALID;
    }

    char byte_string[2] = { c, '\0' };

    if (file->write_function) {
        file->write_function(file, byte_string, 1);
    }

    return FILE_SUCCESS;
}

void *file_get_buffer(File *file) {
    return file->state;
}

uint32_t file_get_pointer(File *file) {
    return file->file_pointer;
}

File *file_descriptor_to_pointer(FileDescriptor descriptor) {
    return &files[descriptor];
}
