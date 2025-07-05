#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

typedef struct {
    char* source;
    char* target;
} link;

typedef struct {
    char** module;
    size_t moduleCount;
    char** pacman;
    size_t pacmanCount;
    char** yay;
    size_t yayCount;
} dep;

typedef struct {
    int level;
    char* version;
    dep deps;
    link* links;
} module;

int main(void) {
    module mod;

    mod.deps.moduleCount = 3;
    mod.deps.module = (char**)malloc(mod.deps.moduleCount * sizeof(char*));

    // parse module
    mod.deps.module[0] = strdup("wofi");
    mod.deps.module[1] = strdup("nvim");
    mod.deps.module[2] = strdup("zsh");

    printf("%zu\n", mod.deps.moduleCount);
    for (size_t i = 0; i < mod.deps.moduleCount; i++) {
        printf("%s ", mod.deps.module[i]);
    }
    printf("\n");

    for (size_t i = 0; i < mod.deps.moduleCount; i++) {
        free(mod.deps.module[i]);
    }
    free(mod.deps.module);

    return 0;
}
