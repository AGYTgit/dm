#ifndef STRUCT_DIS_H
#define STRUCT_DIS_H

#include "../structs/flags.h"
#include "../structs/command.h"
#include "../structs/config.h"
#include "../resources/exPaths.h"

typedef struct {
    flags flags;
    command cmd;
    config conf;
    extraPaths exPaths;
} disArgs;

#endif // STRUCT_DIS_H
