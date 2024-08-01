#include "sys_file.h"

SYSCALL_DEFINE3(3, descriptor, data, size) {
    int32_t ret = file_write(descriptor, (char*)data, size);

    SYSCALL_RETURN(ret);
}
