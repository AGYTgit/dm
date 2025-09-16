#ifndef STRUCT_CONFIG_H
#define STRUCT_CONFIG_H

#include <stdbool.h>

typedef struct config config;

typedef enum {
    CONFIG_ERROR_NONE,
    CONFIG_ERROR_CONFIG_FILE_NOT_FOUND,
    CONFIG_ERROR_YAML_PARSER_INIT_FAILED,
    CONFIG_ERROR_YAML_PARSER_FAILED,
    CONFIG_ERROR_MEMORY_ALLOCATION_FAILED,
    CONFIG_ERROR_INCORRECT_BOOL_TYPE
} configErrorType;

typedef struct {
    configErrorType type;
    char* value;
} configError;

typedef struct app {
    char* name;
    char* version;
    char* description;
} configApp;

typedef struct paths {
    char* repo;
    char* template;
    char* backup;
    char* log;
} configPaths;

typedef struct behavior {
    bool autoGit;
    bool autoApply;
    bool promptForConfirmation;
} configBehavior;

typedef struct config {
    configApp app;
    configPaths paths;
    configBehavior behavior;

    configError error;
} config;

#endif // STRUCT_CONFIG_H
