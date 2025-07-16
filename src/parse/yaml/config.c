#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>

#include "config.h"

static void setConfigError(config* conf, configErrorType type, const char* value) {
    conf->error.type = type;

    free(conf->error.value);
    if (value) {
        conf->error.value = strdup(value);
    } else {
        conf->error.value = NULL;
    }
}

config parseConfig(const char* filePath) {
    config conf = {0};
    FILE* file = NULL;
    yaml_parser_t parser;
    yaml_event_t event;

    conf.error.type = configErrorNone;

    file = fopen(filePath, "rb");
    if (!file) {
        setConfigError(&conf, configErrorConfigFileNotFound, filePath);
        return conf;
    }

    if (!yaml_parser_initialize(&parser)) {
        setConfigError(&conf, configErrorYamlParserInitFailed, NULL);
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
            setConfigError(&conf, configErrorYamlParserFailed, parser.problem ? parser.problem : "Unknown YAML parsing error");
            yaml_event_delete(&event);
            break;
        }

        switch (event.type) {
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
                            conf.paths.repo = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "backup") == 0) {
                            free(conf.paths.backup);
                            conf.paths.backup = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "log") == 0) {
                            free(conf.paths.log);
                            conf.paths.log = strdup((char*)event.data.scalar.value);
                        }
                    } else if (state == stateInBehavior) {
                        if (strcmp(currentKey, "autoGit") == 0) {
                            char* value = (char*)event.data.scalar.value;
                            if (strcmp(value, "true") == 0) {
                                conf.behavior.autoGit = true;
                            } else if (strcmp(value, "false") == 0) {
                                conf.behavior.autoGit = true;
                            } else {
                                setConfigError(&conf, configErrorIncorrectBoolType, value);
                            }
                        } else if (strcmp(currentKey, "promptForConfirmation") == 0) {
                            char* value = (char*)event.data.scalar.value;
                            if (strcmp(value, "true") == 0) {
                                conf.behavior.promptForConfirmation = true;
                            } else if (strcmp(value, "false") == 0) {
                                conf.behavior.promptForConfirmation = true;
                            } else {
                                setConfigError(&conf, configErrorIncorrectBoolType, value);
                            }
                        }
                    }
                    free(currentKey);
                    currentKey = NULL;
                }
                break;
        }
        yaml_event_delete(&event);
        if (conf.error.type != configErrorNone) {
            done = 1;
        }
    }

    free(currentKey);

    yaml_parser_delete(&parser);
    fclose(file);
    return conf;
}

void freeConfig(config* conf) {
    if (!conf) {
        return;
    }

    free(conf->app.name);
    free(conf->app.version);
    free(conf->app.description);

    free(conf->paths.repo);
    free(conf->paths.backup);
    free(conf->paths.log);

    free(conf->error.value);
}
