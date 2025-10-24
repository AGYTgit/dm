#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>

#include "../log/util.h"
#include "../file/util.h"

#include "parseYaml.h"

int getDungeonConf(dungeonConf* conf, const char* filePath) {
    *conf = parseDungeonConf(filePath);

    if (conf->error.type) {
        logError("[%d] while parsing dungeonConf %s", conf->error.type, conf->error.value ? conf->error.value : "");
        return 1;
    }

    return 0;
}

static void setDungeonConfErr(dungeonConf* conf, dungeonConfErrorType type, const char* value) {
    conf->error.type = type;

    free(conf->error.value);
    if (value) {
        conf->error.value = strdup(value);
        if (!conf->error.value) {
            conf->error.type = DUNGEON_CONF_ERR_MEMORY_ALLOCATION_FAILED;
        }
    } else {
        conf->error.value = NULL;
    }
}

dungeonConf parseDungeonConf(const char* filePath) {
    dungeonConf conf = {0};
    FILE* file = NULL;
    yaml_parser_t parser;
    yaml_event_t event;

    conf.error.type = DUNGEON_CONF_ERR_NONE;

    file = fopen(filePath, "rb");
    if (!file) {
        setDungeonConfErr(&conf, DUNGEON_CONF_ERR_CONFIG_FILE_NOT_FOUND, filePath);
        return conf;
    }

    if (!yaml_parser_initialize(&parser)) {
        setDungeonConfErr(&conf, DUNGEON_CONF_ERR_YAML_PARSER_INIT_FAILED, NULL);
        fclose(file);
        return conf;
    }

    yaml_parser_set_input_file(&parser, file);

    int done = 0;
    char* currentKey = NULL;
    enum {
        stateNone,
        stateInState,
        stateInModules,
        stateInModulesAll,
        stateInModulesLoaded
    } state = stateNone;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            setDungeonConfErr(&conf, DUNGEON_CONF_ERR_YAML_PARSER_FAILED, parser.problem ? parser.problem : "Unknown YAML parsing error");
            yaml_event_delete(&event);
            break;
        }

        switch (event.type) {
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                done = 1;
            case YAML_NO_EVENT:
            case YAML_ALIAS_EVENT:
            case YAML_STREAM_START_EVENT:
            case YAML_DOCUMENT_START_EVENT:
                break;
            case YAML_SEQUENCE_START_EVENT:
                if (state == stateInModules && currentKey) {
                    if (strcmp(currentKey, "all") == 0) {
                        state = stateInModulesAll;
                    } else if (strcmp(currentKey, "loaded") == 0) {
                        state = stateInModulesLoaded;
                    }
                }
                free(currentKey);
                currentKey = NULL;
                break;
            case YAML_SEQUENCE_END_EVENT:
                if (state == stateInModulesAll ||
                    state == stateInModulesLoaded) {
                    state = stateInModules;
                }
                break;
            case YAML_MAPPING_START_EVENT:
                if (state == stateNone && currentKey) {
                    if (strcmp(currentKey, "state") == 0) {
                        state = stateInState;
                    }
                } else if (state == stateInState && currentKey) {
                    if (strcmp(currentKey, "modules") == 0) {
                        state = stateInModules;
                    }
                }
                free(currentKey);
                currentKey = NULL;
                break;
            case YAML_MAPPING_END_EVENT:
                if (state == stateInModules) {
                    state = stateInState;
                } else if (state == stateInModulesAll ||
                           state == stateInModulesLoaded) {
                    state = stateInModules;
                }
                break;
            case YAML_SCALAR_EVENT:
                if (!currentKey) {
                    currentKey = strdup((char*)event.data.scalar.value);

                    if (state == stateInModulesAll ||
                        state == stateInModulesLoaded) {
                        if (state == stateInModulesAll) {
                            conf.moduleCount++;
                            conf.modules = realloc(conf.modules, (conf.moduleCount) * sizeof(dungeonModule));
                            conf.modules[conf.moduleCount - 1].name = strdup((char*)event.data.scalar.value);
                            conf.modules[conf.moduleCount - 1].state = 0;
                        } else {
                            for (size_t i = 0; i < conf.moduleCount; ++i) {
                                if (strcmp(conf.modules[i].name, (char*)event.data.scalar.value) == 0) {
                                    conf.modules[i].state = 1;
                                    break;
                                }
                            }
                        }
                        free(currentKey);
                        currentKey = NULL;
                    }
                } else {
                    if (state == stateInState) {
                        if (strcmp(currentKey, "profile") == 0) {
                            conf.profile = strdup((char*)event.data.scalar.value);
                            if (!conf.profile) { setDungeonConfErr(&conf, DUNGEON_CONF_ERR_MEMORY_ALLOCATION_FAILED, "dungeonConf profile allocation failed"); done = 1; }
                        } else if (strcmp(currentKey, "theme") == 0) {
                            conf.theme = strdup((char*)event.data.scalar.value);
                            if (!conf.theme) { setDungeonConfErr(&conf, DUNGEON_CONF_ERR_MEMORY_ALLOCATION_FAILED, "dungeonConf theme allocation failed"); done = 1; }
                        }
                    }
                    free(currentKey);
                    currentKey = NULL;
                }
                break;
        }
        yaml_event_delete(&event);
        if (conf.error.type != DUNGEON_CONF_ERR_NONE) {
            done = 1;
        }
    }

    free(currentKey);

    yaml_parser_delete(&parser);
    fclose(file);
    return conf;
}
