#include "parseModule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

static void addStringToArray(char*** array, size_t* count, const char* str) {
    char** tempArray = realloc(*array, (*count + 1) * sizeof(char*));
    if (!tempArray) {
        perror("Failed to reallocate string array");
        exit(EXIT_FAILURE);
    }
    *array = tempArray;

    (*array)[*count] = strdup(str);
    if (!(*array)[*count]) {
        perror("Failed to duplicate string");
        exit(EXIT_FAILURE);
    }
    (*count)++;
}

static void addLinkToArray(link** array, size_t* count, const link* op) {
    link* tempArray = realloc(*array, (*count + 1) * sizeof(link));
    if (!tempArray) {
        perror("Failed to reallocate link array");
        exit(EXIT_FAILURE);
    }
    *array = tempArray;

    (*array)[*count] = *op;
    (*count)++;
}

typedef enum {
    contextNone,        // For the outermost document mapping (e.g., the 'module:' key's container)
    contextModuleConf,  // For the actual module configuration data
    contextDeps,
    contextDepsModule,
    contextDepsPacman,
    contextDepsYay,
    contextLinks,
    contextLinkItem
} parserContextType;

typedef struct {
    parserContextType type;
    char* currentKey;
    moduleConf* modConfPtr;
    dep* depsPtr;
    link* linkItemPtr;
} ContextStackItem;

static ContextStackItem contextStack[32];
static int stackTop = -1;

static void pushContext(parserContextType type, moduleConf* modConf, dep* deps, link* linkItem) {
    if (stackTop >= 31) {
        fprintf(stderr, "Error: Context stack overflow! Max depth exceeded.\n");
        exit(EXIT_FAILURE);
    }
    stackTop++;
    contextStack[stackTop].type = type;
    contextStack[stackTop].currentKey = NULL;
    contextStack[stackTop].modConfPtr = modConf;
    contextStack[stackTop].depsPtr = deps;
    contextStack[stackTop].linkItemPtr = linkItem;
}

static void popContext() {
    if (stackTop < 0) {
        fprintf(stderr, "Error: Context stack underflow! Attempted to pop from empty stack.\n");
        exit(EXIT_FAILURE);
    }
    if (contextStack[stackTop].currentKey) {
        free(contextStack[stackTop].currentKey);
        contextStack[stackTop].currentKey = NULL;
    }
    stackTop--;
}

static ContextStackItem* peekContext() {
    if (stackTop < 0) return NULL;
    return &contextStack[stackTop];
}

int parseModuleConf(const char* filepath, moduleConf* modConf) {
    FILE* fh = NULL;
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;
    int success = 0;

    memset(modConf, 0, sizeof(moduleConf));
    modConf->deps.module = NULL;
    modConf->deps.pacman = NULL;
    modConf->deps.yay = NULL;
    modConf->links = NULL;
    modConf->version = NULL;
    modConf->modules = NULL;
    modConf->moduleCount = 0;

    fh = fopen(filepath, "rb");
    if (!fh) {
        fprintf(stderr, "Failed to open '%s'. Please ensure the file exists.\n", filepath);
        return -1;
    }

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser!\n");
        fclose(fh);
        return -1;
    }
    yaml_parser_set_input_file(&parser, fh);

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Parser error: %s\n", parser.problem);
            yaml_parser_delete(&parser);
            fclose(fh);
            return -1;
        }

        ContextStackItem* currentCtx = peekContext();

        switch (event.type) {
            case YAML_STREAM_START_EVENT:
            case YAML_DOCUMENT_START_EVENT:
                break;

            case YAML_MAPPING_START_EVENT:
                if (stackTop == -1) {
                    // This is the very first mapping (the root document mapping containing 'module:')
                    pushContext(contextNone, NULL, NULL, NULL);
                } else if (currentCtx && currentCtx->type == contextNone && currentCtx->currentKey && strcmp(currentCtx->currentKey, "module") == 0) {
                    // We just parsed the 'module' key, and this is the MAPPING_START for its value (the actual moduleConf)
                    pushContext(contextModuleConf, modConf, NULL, NULL);
                    // Clear the 'module' key from the contextNone, as we've handled its value.
                    free(currentCtx->currentKey);
                    currentCtx->currentKey = NULL;
                } else if (currentCtx && currentCtx->type == contextModuleConf && currentCtx->currentKey && strcmp(currentCtx->currentKey, "deps") == 0) {
                    pushContext(contextDeps, modConf, &modConf->deps, NULL);
                } else if (currentCtx && currentCtx->type == contextLinks) {
                    link newLinkItem = { .source = NULL, .target = NULL };
                    pushContext(contextLinkItem, modConf, NULL, &newLinkItem);
                } else if (currentCtx && currentCtx->type == contextModuleConf && currentCtx->currentKey && strcmp(currentCtx->currentKey, "modules") == 0) {
                    // TODO: Implement parsing for the 'modules' sequence
                }
                currentCtx = peekContext();
                break;

            case YAML_MAPPING_END_EVENT:
                if (currentCtx && currentCtx->type == contextLinkItem) {
                    addLinkToArray(&modConf->links, &modConf->linkCount, currentCtx->linkItemPtr);
                }
                popContext();
                break;

            case YAML_SEQUENCE_START_EVENT:
                if (currentCtx && currentCtx->type == contextDeps && currentCtx->currentKey) {
                    if (strcmp(currentCtx->currentKey, "module") == 0) {
                        pushContext(contextDepsModule, modConf, &modConf->deps, NULL);
                    } else if (strcmp(currentCtx->currentKey, "pacman") == 0) {
                        pushContext(contextDepsPacman, modConf, &modConf->deps, NULL);
                    } else if (strcmp(currentCtx->currentKey, "yay") == 0) {
                        pushContext(contextDepsYay, modConf, &modConf->deps, NULL);
                    }
                } else if (currentCtx && currentCtx->type == contextModuleConf && currentCtx->currentKey && strcmp(currentCtx->currentKey, "links") == 0) {
                    pushContext(contextLinks, modConf, NULL, NULL);
                } else if (currentCtx && currentCtx->type == contextModuleConf && currentCtx->currentKey && strcmp(currentCtx->currentKey, "modules") == 0) {
                    // TODO: Implement parsing for the 'modules' sequence
                }
                currentCtx = peekContext();
                break;

            case YAML_SEQUENCE_END_EVENT:
                popContext();
                break;

            case YAML_SCALAR_EVENT: {
                const char* scalarValue = (const char*)event.data.scalar.value;

                currentCtx = peekContext();
                if (!currentCtx) {
                    fprintf(stderr, "Error: Scalar event received with no active context. This should not happen with current logic.\n");
                    done = 1;
                    success = 0;
                    break;
                }

                // If no current key is stored for this mapping, this scalar IS the key
                if ((currentCtx->type == contextNone || currentCtx->type == contextModuleConf || currentCtx->type == contextDeps || currentCtx->type == contextLinkItem) && currentCtx->currentKey == NULL) {
                    currentCtx->currentKey = strdup(scalarValue);
                    if (!currentCtx->currentKey) {
                        perror("Failed to duplicate key string");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // This scalar is a value. Store it based on the current context and the stored key.
                    if (currentCtx->type == contextModuleConf) {
                        if (strcmp(currentCtx->currentKey, "level") == 0) {
                            modConf->level = atoi(scalarValue);
                        } else if (strcmp(currentCtx->currentKey, "version") == 0) {
                            modConf->version = strdup(scalarValue);
                            if (!modConf->version) {
                                perror("Failed to duplicate version string");
                                exit(EXIT_FAILURE);
                            }
                        } else if (strcmp(currentCtx->currentKey, "moduleCount") == 0) {
                             // This is likely an implied count; typically not explicitly in YAML
                        }
                    } else if (currentCtx->type == contextDepsModule) {
                        addStringToArray(&modConf->deps.module, &modConf->deps.moduleCount, scalarValue);
                    } else if (currentCtx->type == contextDepsPacman) {
                        addStringToArray(&modConf->deps.pacman, &modConf->deps.pacmanCount, scalarValue);
                    } else if (currentCtx->type == contextDepsYay) {
                        addStringToArray(&modConf->deps.yay, &modConf->deps.yayCount, scalarValue);
                    } else if (currentCtx->type == contextLinkItem) {
                        if (strcmp(currentCtx->currentKey, "source") == 0) {
                            currentCtx->linkItemPtr->source = strdup(scalarValue);
                            if (!currentCtx->linkItemPtr->source) {
                                perror("Failed to duplicate link source string");
                                exit(EXIT_FAILURE);
                            }
                        } else if (strcmp(currentCtx->currentKey, "target") == 0) {
                            currentCtx->linkItemPtr->target = strdup(scalarValue);
                            if (!currentCtx->linkItemPtr->target) {
                                perror("Failed to duplicate link target string");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    // After processing a value, clear the currentKey for the next key-value pair in this mapping.
                    if (currentCtx->currentKey) {
                        free(currentCtx->currentKey);
                        currentCtx->currentKey = NULL;
                    }
                }
                break;
            }

            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                done = 1;
                success = 1;
                break;

            default:
                break;
        }
        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    return success ? 0 : -1;
}

void freeModuleConf(moduleConf* modConf) {
    if (!modConf) return;

    if (modConf->version) {
        free(modConf->version);
        modConf->version = NULL;
    }

    if (modConf->deps.module) {
        for (size_t i = 0; i < modConf->deps.moduleCount; i++) {
            if (modConf->deps.module[i]) free(modConf->deps.module[i]);
        }
        free(modConf->deps.module);
        modConf->deps.module = NULL;
        modConf->deps.moduleCount = 0;
    }

    if (modConf->deps.pacman) {
        for (size_t i = 0; i < modConf->deps.pacmanCount; i++) {
            if (modConf->deps.pacman[i]) free(modConf->deps.pacman[i]);
        }
        free(modConf->deps.pacman);
        modConf->deps.pacman = NULL;
        modConf->deps.pacmanCount = 0;
    }

    if (modConf->deps.yay) {
        for (size_t i = 0; i < modConf->deps.yayCount; i++) {
            if (modConf->deps.yay[i]) free(modConf->deps.yay[i]);
        }
        free(modConf->deps.yay);
        modConf->deps.yay = NULL;
        modConf->deps.yayCount = 0;
    }

    if (modConf->links) {
        for (size_t i = 0; i < modConf->linkCount; i++) {
            if (modConf->links[i].source) free(modConf->links[i].source);
            if (modConf->links[i].target) free(modConf->links[i].target);
        }
        free(modConf->links);
        modConf->links = NULL;
        modConf->linkCount = 0;
    }

    // TODO: Implement freeing for nested modules if they are parsed
    if (modConf->modules) {
        for (size_t i = 0; i < modConf->moduleCount; i++) {
            // If modules are allocated individually (e.g., each new moduleConf)
            // freeModuleConf(&modConf->modules[i]);
            // Or if they are pointers to structs:
            // if (modConf->modules[i]) freeModuleConf(modConf->modules[i]);
        }
        // free(modConf->modules);
        modConf->modules = NULL;
        modConf->moduleCount = 0;
    }
}
