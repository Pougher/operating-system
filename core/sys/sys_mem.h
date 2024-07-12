#ifndef CORE_SYS_MEM_H
#define CORE_SYS_MEM_H

#include "../mem.h"
#include "../syscall.h"
#include "../allocator.h"

// MALLOC(1) system call that returns a 16 byte aligned address pointing to free
// memory with the size specified
SYSCALL_PROTO_DEFINE(0);

// FREE(1) system call that frees a 16 byte aligned address returned by a malloc
// system call
SYSCALL_PROTO_DEFINE(1);

// CALLOC(2) system call that returns a 16 byte aligned address that is default
// initialized to 0. The size of the memory allocated is the product of the
// number of blocks and the size of each block
SYSCALL_PROTO_DEFINE(2);

#endif
