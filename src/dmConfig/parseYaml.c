#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>

#include "../log/util.h"
#include "../file/util.h"

#include "parseYaml.h"

int getConfig(config* conf, const char* configPath) {
    *conf = parseConfig(configPath);

    if (conf->error.type) {
        logError("[%d] while parsing config %s", conf->error.type, conf->error.value ? conf->error.value : "");
        return 1;
    }

    return 0;
}

int printAppConf(config conf) {
    if (conf.error.type) {
        logBlank("ERROR [%d]: %s\n", conf.error.type, conf.error.value ? conf.error.value : "");
        return 1;
    }

    logBlank("\n");
    logBlank("name:        %s\n", conf.app.name);
    logBlank("version:     %s\n", conf.app.version);
    logBlank("description: %s\n", conf.app.description);
    logBlank("\n");
    logBlank("repo:     %s\n", conf.paths.repo);
    logBlank("template: %s\n", conf.paths.template);
    logBlank("backup:   %s\n", conf.paths.backup);
    logBlank("log:      %s\n", conf.paths.log);
    logBlank("\n");
    logBlank("autoGit:               %s\n", conf.behavior.autoGit ? "true" : "false");
    logBlank("autoApply:             %s\n", conf.behavior.autoApply ? "true" : "false");
    logBlank("promptForConfirmation: %s\n", conf.behavior.promptForConfirmation ? "true" : "false");
    logBlank("\n");

    return 0;
}

static void setConfigError(config* conf, configErrorType type, const char* value) {
    conf->error.type = type;

    free(conf->error.value);
    if (value) {
        conf->error.value = strdup(value);
        if (!conf->error.value) {
            conf->error.type = CONFIG_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    } else {
        conf->error.value = NULL;
    }
}

config parseConfig(const char* filePath) {
    config conf = {0};
    FILE* file = NULL;
    yaml_parser_t parser;
    yaml_event_t event;

    conf.error.type = CONFIG_ERROR_NONE;


    file = fopen(filePath, "rb");
    if (!file) {
        setConfigError(&conf, CONFIG_ERROR_CONFIG_FILE_NOT_FOUND, filePath);
        return conf;
    }

    if (!yaml_parser_initialize(&parser)) {
        setConfigError(&conf, CONFIG_ERROR_YAML_PARSER_INIT_FAILED, NULL);
        fclose(file);
        return conf;
    }

    yaml_parser_set_input_file(&parser, file);

    int done = 0;
    char* currentKey = NULL;
    enum {
        stateNone,
        stateInConfig,
        stateInApp,
        stateInPaths,
        stateInBehavior
    } state = stateNone;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            setConfigError(&conf, CONFIG_ERROR_YAML_PARSER_FAILED, parser.problem ? parser.problem : "Unknown YAML parsing error");
            yaml_event_delete(&event);
            break;
        }

        switch (event.type) {
            case YAML_NO_EVENT:
            case YAML_ALIAS_EVENT:
                break;
            case YAML_STREAM_START_EVENT:
            case YAML_DOCUMENT_START_EVENT:
                break;
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                done = 1;
                break;
            case YAML_MAPPING_START_EVENT:
                if (state == stateNone) {
                    state = stateInConfig;
                } else if (state == stateInConfig && currentKey) {
                    if (strcmp(currentKey, "app") == 0) {
                        state = stateInApp;
                    } else if (strcmp(currentKey, "paths") == 0) {
                        state = stateInPaths;
                    } else if (strcmp(currentKey, "behavior") == 0) {
                        state = stateInBehavior;
                    }
                    free(currentKey);
                    currentKey = NULL;
                }
                break;
            case YAML_MAPPING_END_EVENT:
                if (state == stateInApp ||
                    state == stateInPaths ||
                    state == stateInBehavior) {
                    state = stateInConfig;
                }
                break;
            case YAML_SEQUENCE_START_EVENT:
            case YAML_SEQUENCE_END_EVENT:
                break;
            case YAML_SCALAR_EVENT:
                if (!currentKey) {
                    if (state == stateInConfig ||
                        state == stateInApp ||
                        state == stateInPaths ||
                        state == stateInBehavior) {
                        currentKey = strdup((char*)event.data.scalar.value);
                    }
                } else {
                    if (state == stateInApp) {
                        if (strcmp(currentKey, "name") == 0) {
                            free(conf.app.name);
                            conf.app.name = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "version") == 0) {
                            free(conf.app.version);
                            conf.app.version = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "description") == 0) {
                            free(conf.app.description);
                            conf.app.description = strdup((char*)event.data.scalar.value);
                        }
                    } else if (state == stateInPaths) {
                        if (strcmp(currentKey, "repo") == 0) {
                            free(conf.paths.repo);
                            expandEnvVars(&conf.paths.repo, (char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "template") == 0) {
                            free(conf.paths.template);
                            expandEnvVars(&conf.paths.template, (char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "backup") == 0) {
                            free(conf.paths.backup);
                            expandEnvVars(&conf.paths.backup, (char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "log") == 0) {
                            free(conf.paths.log);
                            expandEnvVars(&conf.paths.log, (char*)event.data.scalar.value);
                        }
                    } else if (state == stateInBehavior) {
                        if (strcmp(currentKey, "autoGit") == 0) {
                            char* value = (char*)event.data.scalar.value;
                            if (strcmp(value, "true") == 0) {
                                conf.behavior.autoGit = true;
                            } else if (strcmp(value, "false") == 0) {
                                conf.behavior.autoGit = false;
                            } else {
                                setConfigError(&conf, CONFIG_ERROR_INCORRECT_BOOL_TYPE, value);
                            }
                        } else if (strcmp(currentKey, "autoApply") == 0) {
                            char* value = (char*)event.data.scalar.value;
                            if (strcmp(value, "true") == 0) {
                                conf.behavior.autoApply = true;
                            } else if (strcmp(value, "false") == 0) {
                                conf.behavior.autoApply = false;
                            } else {
                                setConfigError(&conf, CONFIG_ERROR_INCORRECT_BOOL_TYPE, value);
                            }
                        } else if (strcmp(currentKey, "promptForConfirmation") == 0) {
                            char* value = (char*)event.data.scalar.value;
                            if (strcmp(value, "true") == 0) {
                                conf.behavior.promptForConfirmation = true;
                            } else if (strcmp(value, "false") == 0) {
                                conf.behavior.promptForConfirmation = false;
                            } else {
                                setConfigError(&conf, CONFIG_ERROR_INCORRECT_BOOL_TYPE, value);
                            }
                        }
                    }
                    free(currentKey);
                    currentKey = NULL;
                }
                break;
        }
        yaml_event_delete(&event);
        if (conf.error.type != CONFIG_ERROR_NONE) {
            done = 1;
        }
    }

    free(currentKey);

    yaml_parser_delete(&parser);
    fclose(file);
    return conf;
}
