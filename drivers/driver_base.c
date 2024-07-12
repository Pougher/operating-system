#include "driver_base.h"

void driver_io_wait() {
    port_outb(DRIVER_UNUSED_PORT, 0);
}
