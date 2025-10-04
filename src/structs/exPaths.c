#include <stdlib.h>

#include "exPaths.h"

int freeExtraPaths(extraPaths* exPaths) {
    free(exPaths->help);
    free(exPaths->conf);
}
