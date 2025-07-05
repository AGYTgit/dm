#ifndef PARSE_MODULE_H
#define PARSE_MODULE_H

typedef struct module module;

typedef enum {
    MODULE_ERROR_NONE,
    MODULE_ERROR_CONFIG_FILE_NOT_FOUND,
    MODULE_ERROR_YAML_PARSER_INIT_FAILED,
    MODULE_ERROR_YAML_PARSER_FAILED
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

module parseModule(const char* fp);

void freeModule(module* mod);

#endif // PARSE_MODULE_H
