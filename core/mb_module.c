#include "mb_module.h"

multiboot_module_t *module_get(multiboot_info_t *info, uint32_t index) {
    if (index >= info->mods_count) {
        return NULL;
    }

    multiboot_module_t *mod = (multiboot_module_t*) (
        (uint32_t)info->mods_addr + (uint32_t)KERNEL_VIRTUAL_OFFSET + \
        index * sizeof(multiboot_module_t)
    );
    return mod;
}
