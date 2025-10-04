#include <stdlib.h>

#include "flags.h"

int freeFlags(flags* flags) {
    free(flags->value);
    free(flags->error.value);
}
