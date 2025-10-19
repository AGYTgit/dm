#include <stdlib.h>

#include "core.h"

int freeFlags(flags* flags) {
    free(flags->value);
    free(flags->error.value);

    return 0;
}
