#ifndef CORE_MB_MODULE_H
#define CORE_MB_MODULE_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"
#include "format.h"

#include "../kernel/kinfo.h"

// returns a module at the specified index. If no such module exists in the
// multiboot info struct, return NULL
multiboot_module_t *module_get(multiboot_info_t*, uint32_t);

#endif
