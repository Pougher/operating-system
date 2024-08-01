#ifndef CORE_SYS_FILE_H
#define CORE_SYS_FILE_H

#include "../syscall.h"
#include "../file.h"

// WRITE(3) system call that writes a number of bytes from a pointer to a file
// descriptor. Returns -1 if an invalid file descriptor is provided, and 0 if
// the operation was a success
SYSCALL_PROTO_DEFINE(3);

#endif
