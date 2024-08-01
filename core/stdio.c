#include "file.h"
#include "format.h"

void stdout_write(File *stdout_file, char *data, uint32_t length) {
    (void) stdout_file;
    (void) length;

    printf(data);
}
