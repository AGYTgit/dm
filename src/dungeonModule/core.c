#include <stddef.h>
#include "core.h"

void freeDungeonModule(dungeonModule* mod) {
    if (!mod) {
        return;
    }

    free(mod->name);
    free(mod->version);
    free(mod->path);
    free(mod->error.value);

    for (size_t i = 0; i < mod->deps.module.count; ++i) {
        free(mod->deps.module.value[i]);
    }
    free(mod->deps.module.value);

    for (size_t i = 0; i < mod->deps.pacman.count; ++i) {
        free(mod->deps.pacman.value[i]);
    }
    free(mod->deps.pacman.value);

    for (size_t i = 0; i < mod->deps.yay.count; ++i) {
        free(mod->deps.yay.value[i]);
    }
    free(mod->deps.yay.value);

    for (size_t i = 0; i < mod->links.count; ++i) {
        free(mod->links.source[i]);
        free(mod->links.target[i]);
    }
    free(mod->links.source);
    free(mod->links.target);

    for (size_t i = 0; i < mod->subModCount; ++i) {
        freeDungeonModule(&mod->subMods[i]); // TODO: check return value for error
    }

    free(mod->subMods);
}
