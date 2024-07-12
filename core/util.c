#include "util.h"

void kpanic(char *message) {
    printf("\x8a");
    printf(message);
    klock();
}
