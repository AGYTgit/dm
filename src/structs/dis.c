#include "dis.h"
#include "flags.h"
#include "command.h"
#include "config.h"
#include "exPaths.h"

int freeDisArgs(disArgs* data) {
    if (!data) {
        return 0;
    }

    freeFlags(&data->flags);
    freeCommand(&data->cmd);
    freeConfig(&data->conf);
    freeExtraPaths(&data->exPaths);

    return 0;
}

