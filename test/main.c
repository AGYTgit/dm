#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseModule.h"

int main(void) {
    char* fh = "/home/agyt/temp/DotMod/test/module.yaml";

    module mod = parseModule(fh);

    if (mod.error.type) {
        printf("ERROR [%d]: %s\n", mod.error.type, mod.error.value ? mod.error.value : "");
        return 1;
    }

    printf("\n");

    printf("name: %s\n", mod.name);
    printf("version: %s\n", mod.version);
    printf("path: \'%s\'\n", mod.path);

    printf("level: %d\n", mod.conf.level);

    printf("%s", (mod.deps.module.count + mod.deps.pacman.count + mod.deps.yay.count) > 0 ? "deps:\n" : "");
    printf("%s", mod.deps.module.count > 0 ? "    module:\n" : "");
    for (size_t i = 0; i < mod.deps.module.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.module.value[i]);
    }
    printf("%s", mod.deps.pacman.count > 0 ? "    pacman:\n" : "");
    for (size_t i = 0; i < mod.deps.pacman.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.pacman.value[i]);
    }
    printf("%s", mod.deps.yay.count > 0 ? "    yay:\n" : "");
    for (size_t i = 0; i < mod.deps.yay.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.yay.value[i]);
    }

    printf("%s", mod.links.count > 0 ? "links:\n" : "");
    for (size_t i = 0; i < mod.links.count; ++i) {
        printf("    source: %s -> target: %s\n", mod.links.source[i], mod.links.target[i]);
    }

    printf("\n");

    return 0;
}
