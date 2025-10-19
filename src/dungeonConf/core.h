#ifndef STRUCT_DUNGEON_CONF_H
#define STRUCT_DUNGEON_CONF_H

typedef enum {
    CONFIG_ERROR_NONE,
    CONFIG_ERROR_CONFIG_FILE_NOT_FOUND,
    CONFIG_ERROR_YAML_PARSER_INIT_FAILED,
    CONFIG_ERROR_YAML_PARSER_FAILED,
    CONFIG_ERROR_MEMORY_ALLOCATION_FAILED,
    CONFIG_ERROR_INCORRECT_BOOL_TYPE
} dungeonConfErrorType;

typedef struct {
    configErrorType type;
    char* value;
} dungeonConfError;

typedef struct {
    char* name;
    boolean state;
} dungeonModule ;

typedef struct dungeonConf {
    char* profile;
    char* theme;
    dungeonModule* modules;

    configError error;
} dungeonConf;

int freeConfig(config* conf);

#endif // STRUCT_DUNGEON_CONF_H
