#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "../log/util.h"

#include "parseYaml.h"

static void setModuleError(dungeonModule* mod, moduleErrorType type, const char* value) {
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

static int addToList(countCharPtrPtrPair* list, const char* value) {
    size_t newCount = list->count + 1;
    char** newValues = realloc(list->value, newCount * sizeof(char*));
    if (!newValues) {
        return 1;
    }
    list->value = newValues;
    list->value[list->count] = strdup(value);
    if (!list->value[list->count]) {
        return 1;
    }
    list->count = newCount;
    return 0;
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


dungeonModule parseDungeonModule(const char* filePath) {
    dungeonModule mod = {0};

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
        stateInConfGitIgnore,
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
            case YAML_NO_EVENT:
            case YAML_ALIAS_EVENT:
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
                } else if (state == stateInConf && currentKey) {
                    if (strcmp(currentKey, "gitIgnore") == 0) {
                        state = stateInConfGitIgnore;
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
                if (state == stateInConfGitIgnore) {
                    state = stateInConf;
                } else if (state == stateInModuleDeps ||
                    state == stateInPacmanDeps ||
                    state == stateInYayDeps) {
                    state = stateInDeps;
                } else if (state == stateInCommandsLoad ||
                    state == stateInCommandsUload) {
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
                    } else if (state == stateInConf) {
                        if (strcmp(currentKey, "enable") == 0) {
                            mod.conf.enable = atoi((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "level") == 0) {
                            mod.conf.level = atoi((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "exec") == 0) {
                            mod.conf.exec = atoi((char*)event.data.scalar.value);
                        }
                    } else if (state == stateInLinkEntry) {
                        if (strcmp(currentKey, "source") == 0) {
                            free(currentLinkSource);
                            currentLinkSource = strdup((char*)event.data.scalar.value);
                            if (!currentLinkSource) { setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Link source allocation failed"); done = 1; }
                        } else if (strcmp(currentKey, "target") == 0 && currentLinkSource) {
                            if (!addLink(&mod.links, currentLinkSource, (char*)event.data.scalar.value)) {
                                setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse link");
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
                    } else if (state == stateInConfGitIgnore) {
                        if (addToList(&mod.conf.gitIgnore, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse gitIgnore");
                            done = 1;
                        }
                    } else if (state == stateInModuleDeps) {
                        if (addToList(&mod.deps.module, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse module dependency");
                            done = 1;
                        }
                    } else if (state == stateInPacmanDeps) {
                        if (addToList(&mod.deps.pacman, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse pacman dependency");
                            done = 1;
                        }
                    } else if (state == stateInYayDeps) {
                        if (addToList(&mod.deps.yay, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse yay dependency");
                            done = 1;
                        }
                    } else if (state == stateInCommandsLoad) {
                        if (addToList(&mod.commands.load, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse load command");
                            done = 1;
                        }
                    } else if (state == stateInCommandsUload) {
                        if (addToList(&mod.commands.uload, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, MODULE_ERROR_MEMORY_ALLOCATION_FAILED, "Failed to parse uload command");
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

int printModuleConf(dungeonModule mod) {
    if (mod.error.type) {
        logBlank("ERROR [%d]: %s\n", mod.error.type, mod.error.value ? mod.error.value : "");
        return 1;
    }

    logBlank("\n");

    logBlank("name: %s\n", mod.name);
    logBlank("version: %s\n", mod.version);
    logBlank("path: \'%s\'\n", mod.path);

    logBlank("conf:\n");
    logBlank("    enable: %d\n", mod.conf.enable);
    logBlank("    level: %d\n", mod.conf.level);
    logBlank("    exec: %d\n", mod.conf.exec);

    logBlank("%s", mod.conf.gitIgnore.count > 0 ? "    gitIgnore:\n" : "");
    for (size_t i = 0; i < mod.conf.gitIgnore.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.conf.gitIgnore.value[i]);
    }

    logBlank("%s", (mod.deps.module.count + mod.deps.pacman.count + mod.deps.yay.count) > 0 ? "deps:\n" : "");
    logBlank("%s", mod.deps.module.count > 0 ? "    module:\n" : "");
    for (size_t i = 0; i < mod.deps.module.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.deps.module.value[i]);
    }
    logBlank("%s", mod.deps.pacman.count > 0 ? "    pacman:\n" : "");
    for (size_t i = 0; i < mod.deps.pacman.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.deps.pacman.value[i]);
    }
    logBlank("%s", mod.deps.yay.count > 0 ? "    yay:\n" : "");
    for (size_t i = 0; i < mod.deps.yay.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.deps.yay.value[i]);
    }

    logBlank("%s", mod.links.count > 0 ? "links:\n" : "");
    for (size_t i = 0; i < mod.links.count; ++i) {
        logBlank("    source: %s -> target: %s\n", mod.links.source[i], mod.links.target[i]);
    }

    logBlank("%s", (mod.commands.load.count + mod.commands.uload.count) > 0 ? "commands:\n" : "");
    logBlank("%s", mod.commands.load.count > 0 ? "    load:\n" : "");
    for (size_t i = 0; i < mod.commands.load.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.commands.load.value[i]);
    }
    logBlank("%s", mod.commands.uload.count > 0 ? "    uload:\n" : "");
    for (size_t i = 0; i < mod.commands.uload.count; ++i) {
        logBlank("        [%d]: %s\n", i, mod.commands.uload.value[i]);
    }

    logBlank("\n");

    return 0;
}

