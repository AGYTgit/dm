#ifndef STRUCT_DUNGEON_CONF_H
#define STRUCT_DUNGEON_CONF_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    DUNGEON_CONF_ERR_NONE,
    DUNGEON_CONF_ERR_CONFIG_FILE_NOT_FOUND,
    DUNGEON_CONF_ERR_YAML_PARSER_INIT_FAILED,
    DUNGEON_CONF_ERR_YAML_PARSER_FAILED,
    DUNGEON_CONF_ERR_MEMORY_ALLOCATION_FAILED,
    DUNGEON_CONF_ERR_INCORRECT_BOOL_TYPE
} dungeonConfErrorType;

typedef struct {
    dungeonConfErrorType type;
    char* value;
} dungeonConfError;

typedef struct {
    char* name;
    bool state;
} dungeonModuleForConf;

typedef struct dungeonConf {
    char* profile;
    char* theme;
    dungeonModuleForConf* modules;
    size_t moduleCount;

    dungeonConfError error;
} dungeonConf;

int freeDungeonConf(dungeonConf* conf);

#endif // STRUCT_DUNGEON_CONF_H
