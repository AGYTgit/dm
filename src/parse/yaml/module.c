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
    } else {
        mod->error.value = NULL;
    }
}

static int addToList(depsList* list, const char* value) {
    list->count++;
    char** newValues = realloc(list->value, list->count * sizeof(char*));
    if (!newValues) {
        list->count--;
        return 0;
    }
    list->value = newValues;
    list->value[list->count - 1] = strdup(value);
    if (!list->value[list->count - 1]) {
        list->count--;
        return 0;
    }
    return 1;
}

static int addLink(moduleLinks* links, const char* source, const char* target) {
    links->count++;
    char** newSources = realloc(links->source, links->count * sizeof(char*));
    char** newTargets = realloc(links->target, links->count * sizeof(char*));

    if (!newSources || !newTargets) {
        links->count--;
        free(newSources);
        free(newTargets);
        return 0;
    }
    links->source = newSources;
    links->target = newTargets;

    links->source[links->count - 1] = strdup(source);
    links->target[links->count - 1] = strdup(target);

    if (!links->source[links->count - 1] || !links->target[links->count - 1]) {
        links->count--;
        if (links->source[links->count - 1]) free(links->source[links->count - 1]);
        if (links->target[links->count - 1]) free(links->target[links->count - 1]);
        return 0;
    }
    return 1;
}

module parseModule(const char* filePath) {
    module mod = {0};
    FILE* file = NULL;
    yaml_parser_t parser;
    yaml_event_t event;

    mod.error.type = moduleErrorNone;

    mod.path = strdup(filePath);

    file = fopen(filePath, "rb");
    if (!file) {
        setModuleError(&mod, moduleErrorConfigFileNotFound, filePath);
        return mod;
    }

    if (!yaml_parser_initialize(&parser)) {
        setModuleError(&mod, moduleErrorYamlParserInitFailed, NULL);
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
        stateInLinkEntry
    } state = stateNone;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            setModuleError(&mod, moduleErrorYamlParserFailed, parser.problem ? parser.problem : "Unknown YAML parsing error");
            done = 1;
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
                    state = stateInModule;
                } else if (state == stateInModule && currentKey) {
                    if (strcmp(currentKey, "conf") == 0) {
                        state = stateInConf;
                    } else if (strcmp(currentKey, "deps") == 0) {
                        state = stateInDeps;
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else if (state == stateInLinks) {
                    state = stateInLinkEntry;
                }
                break;
            case YAML_MAPPING_END_EVENT:
                if (state == stateInConf || state == stateInDeps || state == stateInLinks) {
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
                }
                break;
            case YAML_SEQUENCE_END_EVENT:
                if (state == stateInModuleDeps || state == stateInPacmanDeps || state == stateInYayDeps) {
                    state = stateInDeps;
                }
                break;
            case YAML_SCALAR_EVENT:
                if (currentKey) {
                    if (state == stateInModule) {
                        if (strcmp(currentKey, "name") == 0) {
                            free(mod.name);
                            mod.name = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "version") == 0) {
                            free(mod.version);
                            mod.version = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "path") == 0) {
                            free(mod.path);
                            mod.path = strdup((char*)event.data.scalar.value);
                        }
                    } else if (state == stateInConf && strcmp(currentKey, "level") == 0) {
                        mod.conf.level = atoi((char*)event.data.scalar.value);
                    } else if (state == stateInLinkEntry) {
                        if (strcmp(currentKey, "source") == 0) {
                            free(currentLinkSource);
                            currentLinkSource = strdup((char*)event.data.scalar.value);
                        } else if (strcmp(currentKey, "target") == 0 && currentLinkSource) {
                            if (!addLink(&mod.links, currentLinkSource, (char*)event.data.scalar.value)) {
                                setModuleError(&mod, moduleErrorMemoryAllocationFailed, "Memory allocation failed for links");
                                done = 1;
                            }
                            free(currentLinkSource);
                            currentLinkSource = NULL;
                        }
                    }
                    free(currentKey);
                    currentKey = NULL;
                } else {
                    if (state == stateInModule || state == stateInConf || state == stateInDeps || state == stateInLinkEntry) {
                        currentKey = strdup((char*)event.data.scalar.value);
                    } else if (state == stateInModuleDeps) {
                        if (!addToList(&mod.deps.module, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, moduleErrorMemoryAllocationFailed, "Memory allocation failed for module dependencies");
                            done = 1;
                        }
                    } else if (state == stateInPacmanDeps) {
                        if (!addToList(&mod.deps.pacman, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, moduleErrorMemoryAllocationFailed, "Memory allocation failed for pacman dependencies");
                            done = 1;
                        }
                    } else if (state == stateInYayDeps) {
                        if (!addToList(&mod.deps.yay, (char*)event.data.scalar.value)) {
                            setModuleError(&mod, moduleErrorMemoryAllocationFailed, "Memory allocation failed for yay dependencies");
                            done = 1;
                        }
                    }
                }
                break;
        }
        yaml_event_delete(&event);
        if (mod.error.type != moduleErrorNone) {
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
