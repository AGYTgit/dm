#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "parseModule.h"

typedef enum {
    CTX_NONE,
    CTX_MOD,
    CTX_MOD_DEPS,
    CTX_MOD_DEPS_MODULE,
    CTX_MOD_DEPS_PACMAN,
    CTX_MOD_DEPS_YAY,
    CTX_MOD_LINKS,
    CTX_MOD_LINK_ITEM,
} ctxType;

static ctxType ctx;
static char* key;

module parseModule(const char* filepath) {
    ctx = CTX_NONE;
    if (key) { free(key); key = NULL; }

    module mod;
    memset(&mod, 0, sizeof(module));
    mod.path = strdup(filepath);

    FILE* fh = fopen(mod.path, "rb");
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;

    if (!fh) {
        mod.error.type = MODULE_ERROR_CONFIG_FILE_NOT_FOUND;
        return mod;
    }

    if (!yaml_parser_initialize(&parser)) {
        fclose(fh);
        mod.error.type = MODULE_ERROR_YAML_PARSER_INIT_FAILED;
        return mod;
    }
    yaml_parser_set_input_file(&parser, fh);

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            yaml_parser_delete(&parser);
            fclose(fh);
            mod.error.type = MODULE_ERROR_YAML_PARSER_FAILED;
            mod.error.value = strdup((char*)parser.problem);
            return mod;
        }

        switch (event.type) {
            case YAML_STREAM_START_EVENT:
                printf("stream start\n");
                break;
            case YAML_STREAM_END_EVENT:
                printf("stream end\n");
                break;
            case YAML_DOCUMENT_START_EVENT:
                printf("document start\n");
                break;
            case YAML_DOCUMENT_END_EVENT:
                printf("document end\n");
                break;
            case YAML_MAPPING_START_EVENT:
                printf("mapping start\n");

                if (!key) { // temporary solution
                    break;
                }

                switch (ctx) {
                    case CTX_NONE:
                        if (strcmp(key, "module") == 0) {
                            ctx = CTX_MOD;
                        }
                        free(key); key = NULL;
                        break;
                    case CTX_MOD:
                        if (strcmp(key, "deps") == 0) {
                            ctx = CTX_MOD_DEPS;
                        }
                        free(key); key = NULL;
                        break;
                    case CTX_MOD_DEPS:
                        if (strcmp(key, "module") == 0) {
                            ctx = CTX_MOD_DEPS_MODULE;
                        } else if (strcmp(key, "pacman") == 0) {
                            ctx = CTX_MOD_DEPS_PACMAN;
                        } else if (strcmp(key, "yay") == 0) {
                            ctx = CTX_MOD_DEPS_YAY;
                        }
                        free(key); key = NULL;
                        break;

                break;
            case YAML_MAPPING_END_EVENT:
                printf("mapping end\n");

                switch (ctx) {
                    case CTX_MOD:
                        ctx = CTX_NONE;
                        break;

                    case CTX_MOD_DEPS:
                        ctx = CTX_MOD;
                        break;
                    case CTX_MOD_DEPS_MODULE:
                        ctx = CTX_MOD_DEPS;
                        break;
                    case CTX_MOD_DEPS_PACMAN:
                        ctx = CTX_MOD_DEPS;
                        break;
                    case CTX_MOD_DEPS_YAY:
                        ctx = CTX_MOD_DEPS;
                        break;

                    case CTX_MOD_LINKS:
                        ctx = CTX_MOD;
                        break;
                }

                break;
            case YAML_SEQUENCE_START_EVENT:
                printf("sequence start\n");

                if (!key) { // temporary solution
                    break;
                }

                switch (ctx) {
                    case CTX_MOD:
                        if (strcmp(key, "links") == 0) {
                            ctx = CTX_MOD_LINKS;
                        }

                        break;

                break;
            case YAML_SEQUENCE_END_EVENT:
                printf("sequence end\n");

                switch (ctx) {
                    case CTX_MOD_LINK_ITEM:
                        ctx = CTX_MOD_LINKS;
                        free(key); key = NULL;
                        break;

                break;
            case YAML_SCALAR_EVENT:
                char* sc = (char*)event.data.scalar.value;
                printf("scalar: %s\n", sc);

                if (key == NULL) {
                    key = strdup(sc); // might need security check after
                } else {
                    if (ctx == CTX_MOD && key) {
                        if (strcmp(key, "name") == 0) {
                            mod.name = strdup(sc);
                        } else if (strcmp(key, "version") == 0) {
                            mod.version = strdup(sc);
                        } else if (strcmp(key, "level") == 0) {
                            mod.conf.level = atoi(sc);
                        }
                    }

                    free(key);
                    key = NULL;
                }

                break;
        }
        yaml_event_delete(&event);
    }

    free(key);
    yaml_parser_delete(&parser);
    fclose(fh);

    return mod;
}

void freeModule(module* mod) {
    return;
}
