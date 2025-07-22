#ifndef STRUCT_FLAGS_H
#define STRUCT_FLAGS_H

#include <stdbool.h>

typedef enum {
    FLAG_ERROR_NONE,
    FLAG_ERROR_UNKNOWN_FLAG,
} flagErrorType;

typedef struct {
    flagErrorType type;
    char* value;
} flagError;

typedef struct {
    bool custom;
    bool mute;
    bool quiet;
    bool verbose;
    bool force;
    bool help;
    bool version;
    char* value;
    flagError error;
} flags;

#endif // STRUCT_FLAGS_H
