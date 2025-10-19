#include <stdlib.h>

#include "core.h"

int freeConfig(config* conf) {
    if (!conf) {
        return 0;
    }

    free(conf->app.name);
    free(conf->app.version);
    free(conf->app.description);

    free(conf->paths.repo);
    free(conf->paths.template);
    free(conf->paths.backup);
    free(conf->paths.log);

    free(conf->error.value);

    return 0;
}
