#ifndef PARSE_YAML_CONFIG_H
#define PARSE_YAML_CONFIG_H

#include <stdbool.h>

typedef struct config config;

typedef enum {
    configErrorNone,
    configErrorConfigFileNotFound,
    configErrorYamlParserInitFailed,
    configErrorYamlParserFailed,
    configErrorMemoryAllocationFailed,
    configErrorIncorrectBoolType
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
    char* backup;
    char* log;
} configPaths;

typedef struct behavior {
    bool autoGit;
    bool promptForConfirmation;
} configBehavior;

typedef struct config {
    configApp app;
    configPaths paths;
    configBehavior behavior;

    configError error;
} config;

config parseConfig(const char* filePath);

void freeConfig(config* conf);

#endif // PARSE_YAML_CONFIG_H
