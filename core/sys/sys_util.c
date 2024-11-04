#include "sys_util.h"

SYSCALL_DEFINE3(4, a, b, c) {
    (void) a;
    (void) b;
    (void) c;
    printf("hello from shell\n");

    SYSCALL_RETURN(0);
}