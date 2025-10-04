#ifndef STRUCT_DIS_H
#define STRUCT_DIS_H

#include "../structs/flags.h"
#include "../structs/command.h"
#include "../structs/config.h"
#include "../structs/exPaths.h"

typedef struct {
    flags flags;
    command cmd;
    config conf;
    extraPaths exPaths;
} disArgs;

int freeDisArgs(disArgs* data);

#endif // STRUCT_DIS_H
