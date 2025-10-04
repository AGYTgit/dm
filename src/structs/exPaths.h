#ifndef EX_PATHS_H
#define EX_PATHS_H

typedef struct {
    char* help;
    char* conf;
} extraPaths;

int freeExtraPaths(extraPaths* exPaths);

#endif // EX_PATHS_H
