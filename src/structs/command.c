#include <stdlib.h>

#include "command.h"

int freeCommand(command* cmd) {
    free(cmd->value);
    free(cmd->error.value);
}
