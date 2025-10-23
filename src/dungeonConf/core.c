#include "core.h"

int freeDungeonConf(dungeonConf* conf) {
    if (!conf) {
        return 0;
    }

    free(conf->profile);
    free(conf->theme);
    for (size_t i = 0; i < conf->moduleCount; ++i) {
        free(conf->modules[i].name);
    }
    free(conf->modules);
    free(conf->error.value);

    return 0;
}
