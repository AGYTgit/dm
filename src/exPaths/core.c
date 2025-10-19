#include <stdlib.h>

#include "core.h"

int freeExtraPaths(extraPaths* exPaths) {
    free(exPaths->help);
    free(exPaths->conf);

    return 0;
}
