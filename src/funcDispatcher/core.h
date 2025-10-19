#ifndef STRUCT_DIS_H
#define STRUCT_DIS_H

#include "../flag/core.h"
#include "../command/core.h"
#include "../dmConfig/core.h"
#include "../exPaths/core.h"

typedef struct {
    flags flags;
    command cmd;
    config conf;
    extraPaths exPaths;
} disArgs;

int freeDisArgs(disArgs* data);

#endif // STRUCT_DIS_H
