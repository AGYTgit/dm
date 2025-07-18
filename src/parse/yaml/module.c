#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "module.h"

static void setModuleError(module* mod, moduleErrorType type, const char* value) {
    mod->error.type = type;

    free(mod->error.value);
    if (value) {
        mod->error.value = strdup(value);
        if (!mod->error.value) {
            mod->error.type = MODULE_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    } else {
        mod->error.value = NULL;
    }
}

static int addToList(countStrArrPair* list, const char* value) {
    size_t newCount = list->count + 1;
    char** newValues = realloc(list->value, newCount * sizeof(char*));
    if (!newValues) {
        return 0;
    }
    list->value = newValues;
    list->value[list->count] = strdup(value);
    if (!list->value[list->count]) {
        return 0;
    }
    list->count = newCount;
    return 1;
}

static int addLink(moduleLinks* links, const char* source, const char* target) {
    size_t newCount = links->count + 1;
    char** newSources = NULL;
    char** newTargets = NULL;

    newSources = realloc(links->source, newCount * sizeof(char*));
    if (!newSources) {
        return 0;
    }
    links->source = newSources;

    newTargets = realloc(links->target, newCount * sizeof(char*));
    if (!newTargets) {
        if (newSources != links->source) {
            links->source = realloc(newSources, links->count * sizeof(char*));
        }
        return 0;
    }
    links->target = newTargets;

    links->source[links->count] = strdup(source);
    if (!links->source[links->count]) {
        return 0;
    }

    links->target[links->count] = strdup(target);
    if (!links->target[links->count]) {
        free(links->source[links->count]);
        return 0;
    }

    links->count = newCount;
    return 1;
}


module parseModule(const char* filePath) {
    module mod = {0};

    FILE* file = NULL;
    yaml_parser_t parser;
    yaml_event_t event;

    mod.path = strdup(filePath);
    if (!mod.path) {
        setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Initial path allocation failed");
        return mod;
    }

    file = fopen(filePath, "rb");
    if (!file) {
        setModuleError(&mod, MODULE_ERROR_CONFIG_FILE_NOT_FOUND, filePath);
        return mod;
    }

    if (!yaml_parser_initialize(&parser)) {
        setModuleError(&mod, MODULE_ERROR_YAML_PARSER_INIT_FAILED, NULL);
        fclose(file);
        return mod;
    }

    yaml_parser_set_input_file(&parser, file);

    int done = 0;
    char* currentKey = NULL;
    char* currentLinkSource = NULL;

    enum {
        stateNone,
        stateInModule,
        stateInConf,
        stateInDeps,
        stateInModuleDeps,
        stateInPacmanDeps,
        stateInYayDeps,
        stateInLinks,
        stateInLinkEntry,
        stateInCommands,
        stateInCommandsLoad,
        stateInCommandsUload
    } state = stateNone;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            setModuleError(&mod, MODULE_ERROR_YAML_PARSER_FAILED, parser.problem ? parser.problem : "Unknown YAML parsing error");
            done = 1;
            break;
        }

        switch (event.type) {
            case YAML_STREAM_START_EVENT:
            case YAML_DOCUMENT_START_EVENT:
                if (state == stateNone) {
                    state = stateInModule;
                }
                break;

            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                done = 1;
                break;

            case YAML_MAPPING_START_EVENT:
                if (state == stateInModule && currentKey) {
                    if (strcmp(currentKey, "conf") == 0) {
                        state = stateInConf;
                    } else if (strcmp(currentKey, "deps") == 0) {
                        state = stateInDeps;
                    } else if (strcmp(currentKey, "commands") == 0) {
                        state = stateInCommands;
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else if (state == stateInLinks) {
                    state = stateInLinkEntry;
                }
                break;

            case YAML_MAPPING_END_EVENT:
                if (state == stateInConf || state == stateInDeps || state == stateInLinks || state == stateInCommands) {
                    state = stateInModule;
                } else if (state == stateInModuleDeps || state == stateInPacmanDeps || state == stateInYayDeps) {
                    state = stateInDeps;
                } else if (state == stateInLinkEntry) {
                    state = stateInLinks;
                    free(currentLinkSource);
                    currentLinkSource = NULL;
                }
                break;

            case YAML_SEQUENCE_START_EVENT:
                if (state == stateInModule && currentKey) {
                    if (strcmp(currentKey, "links") == 0) {
                        state = stateInLinks;
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else if (state == stateInDeps && currentKey) {
                    if (strcmp(currentKey, "module") == 0) {
                        state = stateInModuleDeps;
                    } else if (strcmp(currentKey, "pacman") == 0) {
                        state = stateInPacmanDeps;
                    } else if (strcmp(currentKey, "yay") == 0) {
                        state = stateInYayDeps;
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else if (state == stateInCommands && currentKey) {
                    if (strcmp(currentKey, "load") == 0) {
                        state = stateInCommandsLoad;
                    } else if (strcmp(currentKey, "uload") == 0) {
                        state = stateInCommandsUload;
                    }
                    free(currentKey);
                    currentKey = NULL;
                }
                break;

            case YAML_SEQUENCE_END_EVENT:
                if (state == stateInModuleDeps || state == stateInPacmanDeps || state == stateInYayDeps) {
                    state = stateInDeps;
                } else if (state == stateInCommandsLoad || state == stateInCommandsUload) {
                    state = stateInCommands;
                } else if (state == stateInLinks) {
                    state = stateInModule;
                }
                break;

            case YAML_SCALAR_EVENT:
                if (currentKey) {
                    if (state == stateInModule) {
                        if (strcmp(currentKey, "name") == 0) {
                            free(mod.name);
                            mod.name = strdup((char*)event.data.scalar.value);
                            if (!mod.name) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Module name allocation failed"); done = 1; }
                        } else if (strcmp(currentKey, "version") == 0) {
                            free(mod.version);
                            mod.version = strdup((char*)event.data.scalar.value);
                            if (!mod.version) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Module version allocation failed"); done = 1; }
                        } else if (strcmp(currentKey, "path") == 0) {
                            free(mod.path);
                            mod.path = strdup((char*)event.data.scalar.value);
                            if (!mod.path) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Module path allocation failed"); done = 1; }
                        }
                    } else if (state == stateInConf && strcmp(currentKey, "level") == 0) {
                        mod.conf.level = atoi((char*)event.data.scalar.value);
                    } else if (state == stateInLinkEntry) {
                        if (strcmp(currentKey, "source") == 0) {
                            free(currentLinkSource);
                            currentLinkSource = strdup((char*)event.data.scalar.value);
                            if (!currentLinkSource) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Link source allocation failed"); done = 1; }
                        } else if (strcmp(currentKey, "target") == 0 && currentLinkSource) {
                            if (!addLink(&mod.links, currentLinkSource, (char*)event.data.scalar.value)) {
                                setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add link");
                                done = 1;
                            }
                            free(currentLinkSource);
                            currentLinkSource = NULL;
                        }
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else {
                    if (state == stateInModule ||
                        state == stateInConf ||
                        state == stateInDeps ||
                        state == stateInLinkEntry ||
                        state == stateInCommands) {
                        currentKey = strdup((char*)event.data.scalar.value);
                        if (!currentKey) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Current key allocation failed"); done = 1; }
                    } else if (state == stateInModuleDeps) {
                        if (!addToList(&mod.deps.module, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add module dependency");
                            done = 1;
                        }
                    } else if (state == stateInPacmanDeps) {
                        if (!addToList(&mod.deps.pacman, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add pacman dependency");
                            done = 1;
                        }
                    } else if (state == stateInYayDeps) {
                        if (!addToList(&mod.deps.yay, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add yay dependency");
                            done = 1;
                        }
                    } else if (state == stateInCommandsLoad) {
                        if (!addToList(&mod.commands.load, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add load command");
                            done = 1;
                        }
                    } else if (state == stateInCommandsUload) {
                        if (!addToList(&mod.commands.uload, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to add unload command");
                            done = 1;
                        }
                    }
                }
                break;
        }
        yaml_event_delete(&event);
        if (mod.error.type != MODULE_ERROR_NONE) {
            done = 1;
        }
    }

    free(currentKey);
    free(currentLinkSource);

    yaml_parser_delete(&parser);
    fclose(file);

    return mod;
}

void freeModule(module* mod) {
    if (!mod) {
        return;
    }

    free(mod->name);
    free(mod->version);
    free(mod->path);
    free(mod->error.value);

    for (size_t i = 0; i < mod->deps.module.count; ++i) {
        free(mod->deps.module.value[i]);
    }
    free(mod->deps.module.value);

    for (size_t i = 0; i < mod->deps.pacman.count; ++i) {
        free(mod->deps.pacman.value[i]);
    }
    free(mod->deps.pacman.value);

    for (size_t i = 0; i < mod->deps.yay.count; ++i) {
        free(mod->deps.yay.value[i]);
    }
    free(mod->deps.yay.value);

    for (size_t i = 0; i < mod->links.count; ++i) {
        free(mod->links.source[i]);
        free(mod->links.target[i]);
    }
    free(mod->links.source);
    free(mod->links.target);

    for (size_t i = 0; i < mod->subModCount; ++i) {
        freeModule(&mod->subMods[i]);
    }

    free(mod->subMods);
}
