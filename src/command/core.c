#include <stdlib.h>

#include "core.h"

int freeCommand(command* cmd) {
    free(cmd->value);
    free(cmd->error.value);

    return 0;
}
