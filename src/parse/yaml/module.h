#ifndef PARSE_YAML_MODULE_H
#define PARSE_YAML_MODULE_H

typedef struct module module;

typedef enum {
    moduleErrorNone,
    moduleErrorConfigFileNotFound,
    moduleErrorYamlParserInitFailed,
    moduleErrorYamlParserFailed,
    moduleErrorMemoryAllocationFailed
} moduleErrorType;

typedef struct {
    moduleErrorType type;
    char* value;
} moduleError;

typedef struct {
    size_t count;
    char** source;
    char** target;
} moduleLinks;

typedef struct {
    size_t count;
    char** value;
} depsList;

typedef struct {
    depsList module;
    depsList pacman;
    depsList yay;
} moduleDeps;

typedef struct {
    int level;
} moduleConf;

typedef struct module {
    char* name;
    char* version;
    char* path;

    moduleConf conf;
    moduleDeps deps;
    moduleLinks links;

    size_t subModCount;
    module* subMods;

    moduleError error;
} module;

module parseModule(const char* filePath);

void freeModule(module* mod);

#endif // PARSE_YAML_MODULE_H
