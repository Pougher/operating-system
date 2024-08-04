#include "file.h"
#include "allocator.h"
#include "format.h"
#include "mem.h"

void stdin_init(File *stdin_file) {
    stdin_file->state = kmalloc(STDIN_BUFFER_SIZE);

    if (!stdin_file->state) {
        kpanic("Couldn't allocate 4096 bytes for STDIN");
    }

    // zero initialize the stdin buffer
    memset(stdin_file->state, 0, STDIN_BUFFER_SIZE);
}

void stdout_write(File *stdout_file, char *data, uint32_t length) {
    (void) stdout_file;
    (void) length;

    printf(data);

    stdout_file->file_pointer += length;
}

void stdin_write(File *stdin_file, char *data, uint32_t length) {
    // if there is enough space in the stdin buffer, write the data to the
    // buffer. Otherwise, cut off the buffer
    if (stdin_file->file_pointer < STDIN_BUFFER_SIZE) {
        const uint32_t capped_length =
            ((stdin_file->file_pointer + length) < STDIN_BUFFER_SIZE) ?
                length : (STDIN_BUFFER_SIZE - stdin_file->file_pointer);

        memcpy(
            (char*)stdin_file->state + stdin_file->file_pointer,
            data,
            capped_length
        );

        stdin_file->file_pointer += capped_length;
    }
}

void stdin_read(File *stdin_file, void *object, uint32_t length) {
    if (stdin_file->file_pointer == 0) {
        return;
    }

    // cap the number of bytes that can be read in the case that length is
    // larger than the number of bytes available
    if (stdin_file->file_pointer < length) {
        length = stdin_file->file_pointer;
    }

    memcpy(object, stdin_file->state, length);
    memcpy(
        stdin_file->state,
        (char*)stdin_file->state + length,
        stdin_file->file_pointer - length
    );

    stdin_file->file_pointer -= length;
}
