#ifndef PARSE_MODULE_H
#define PARSE_MODULE_H

#include <stddef.h>

typedef struct moduleConf moduleConf;

typedef struct {
    char* source;
    char* target;
} link;

typedef struct {
    size_t moduleCount;
    char** module;
    size_t pacmanCount;
    char** pacman;
    size_t yayCount;
    char** yay;
} dep;

typedef struct moduleConf {
    int level;
    char* version;
    dep deps;
    size_t linkCount;
    link* links;
    size_t moduleCount;
    moduleConf* modules;
} moduleConf;

int parseModuleConf(const char* filepath, moduleConf* modConf);

void freeModuleConf(moduleConf* modConf);

#endif // PARSE_MODULE_H
