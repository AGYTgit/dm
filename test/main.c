#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseModule.h"

int main(void) {
    moduleConf modConf;
    const char* moduleFilePath = "module.yaml";

    printf("Attempting to parse '%s'...\n", moduleFilePath);

    if (parseModuleConf(moduleFilePath, &modConf) != 0) {
        fprintf(stderr, "Error: Failed to parse module configuration.\n");
        return EXIT_FAILURE;
    }

    // modConf.level = 1;
    // modConf.version = strdup("1.0");
    // modConf.deps.moduleCount = 0;
    // modConf.deps.pacmanCount = 1;
    // modConf.deps.yayCount = 0;
    // modConf.deps.pacman[0] = strdup("wofi");
    // modConf.moduleCount = 0;
    // modConf.linkCount = 1;
    // modConf.links = (link*)malloc(1 * sizeof(link));
    // modConf.links[0].source = strdup("wofi");
    // modConf.links[0].target = strdup("$HOME/.config/wofi");

    printf("\n--- Successfully Parsed Module Configuration ---\n");
    printf("Level: %d\n", modConf.level);
    printf("Version: %s\n", modConf.version ? modConf.version : "N/A");

    printf("\nDependencies:\n");
    printf("  Module (%zu): ", modConf.deps.moduleCount);
    for (size_t i = 0; i < modConf.deps.moduleCount; i++) {
        printf("%s%s", modConf.deps.module[i], (i == modConf.deps.moduleCount - 1) ? "" : ", ");
    }
    printf("\n");

    printf("  Pacman (%zu): ", modConf.deps.pacmanCount);
    for (size_t i = 0; i < modConf.deps.pacmanCount; i++) {
        printf("%s%s", modConf.deps.pacman[i], (i == modConf.deps.pacmanCount - 1) ? "" : ", ");
    }
    printf("\n");

    printf("  Yay (%zu): ", modConf.deps.yayCount);
    for (size_t i = 0; i < modConf.deps.yayCount; i++) {
        printf("%s%s", modConf.deps.yay[i], (i == modConf.deps.yayCount - 1) ? "" : ", ");
    }
    printf("\n");

    printf("\nLinks (%zu):\n", modConf.linkCount);
    for (size_t i = 0; i < modConf.linkCount; i++) {
        printf("  [%zu] Source: %s, Target: %s\n", i,
                modConf.links[i].source ? modConf.links[i].source : "N/A",
                modConf.links[i].target ? modConf.links[i].target : "N/A");
    }
    printf("\n");

    // These fields are not yet parsed/populated by the current logic in parseModule.c
    printf("Nested Modules (Count: %zu, Not yet parsed):\n", modConf.moduleCount);
    // Add loop here if you implement parsing for nested modules later
    // for (size_t i = 0; i < modConf.moduleCount; i++) {
    //     printf("  Module %zu Level: %d\n", i, modConf.modules[i].level);
    // }
    printf("\n");


    freeModuleConf(&modConf);

    return 0;
}
