#ifndef CORE_SYS_UTIL_H
#define CORE_SYS_UTIL_H

#include "../syscall.h"
#include "../format.h"

// RESERVED(x) syscall that takes an arbitrary number of parameters depending on
// what is being tested
SYSCALL_PROTO_DEFINE(4);

#endif
