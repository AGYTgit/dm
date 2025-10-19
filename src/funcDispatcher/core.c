#include "core.h"
#include "../flag/core.h"
#include "../command/core.h"
#include "../dmConfig/core.h"
#include "../exPaths/core.h"

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
