#ifndef DUNGEON_MODULE_CORE_H
#define DUNGEON_MODULE_CORE_H

typedef struct dungeonModule dungeonModule;

typedef enum {
    MODULE_ERROR_NONE,
    MODULE_ERROR_CONFIG_FILE_NOT_FOUND,
    MODULE_ERROR_YAML_PARSER_INIT_FAILED,
    MODULE_ERROR_YAML_PARSER_FAILED,
    MODULE_ERROR_MEMORY_ALLOCATION_FAILED
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
} countCharPtrPtrPair;

typedef struct {
    countCharPtrPtrPair module;
    countCharPtrPtrPair pacman;
    countCharPtrPtrPair yay;
} moduleDeps;

typedef struct {
    countCharPtrPtrPair load;
    countCharPtrPtrPair uload;
} moduleCommands;

typedef struct {
    int enable;
    int level;
    int exec;
    countCharPtrPtrPair gitIgnore;
} moduleConf;

typedef struct {
    char* name;
    char* version;
    char* path;

    moduleConf conf;
    moduleDeps deps;
    moduleLinks links;
    moduleCommands commands;

    size_t subModCount;
    dungeonModule* subMods;

    moduleError error;
} dungeonModule;

int freeDungeonModule(dungeonModule* mod);

#endif // DUNGEON_MODULE_CORE_H
