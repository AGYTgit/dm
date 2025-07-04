#include <yaml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STATE_NONE,
    STATE_IN_MODULE_MAPPING,
    STATE_EXPECTING_SYMLINKS_SEQUENCE,
    STATE_IN_SYMLINKS_SEQUENCE
} ParserState;

void print_indentation(int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("    ");
    }
}

int main() {
    FILE *fh = NULL;
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;
    int depth = 0;
    int is_key = 0;

    ParserState current_state = STATE_NONE;
    int symlinks_count = 0;

    fh = fopen("module.yaml", "rb");
    if (!fh) {
        fprintf(stderr, "Failed to open 'module.yaml'. Please ensure the file exists.\n");
        return EXIT_FAILURE;
    }

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser!\n");
        fclose(fh);
        return EXIT_FAILURE;
    }

    yaml_parser_set_input_file(&parser, fh);

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Parser error: %s\n", parser.problem);
            yaml_parser_delete(&parser);
            fclose(fh);
            return EXIT_FAILURE;
        }

        switch (event.type) {
            case YAML_STREAM_START_EVENT:
                printf("--- YAML Stream Start ---\n");
                break;
            case YAML_STREAM_END_EVENT:
                printf("--- YAML Stream End ---\n");
                done = 1;
                break;
            case YAML_DOCUMENT_START_EVENT:
                printf("--- Document Start ---\n");
                depth = 0;
                current_state = STATE_IN_MODULE_MAPPING;
                break;
            case YAML_DOCUMENT_END_EVENT:
                printf("--- Document End ---\n");
                break;
            case YAML_MAPPING_START_EVENT:
                print_indentation(depth);
                printf("{\n");
                depth++;
                is_key = 1;

                if (current_state == STATE_IN_SYMLINKS_SEQUENCE) {
                    symlinks_count++;
                }
                break;
            case YAML_MAPPING_END_EVENT:
                depth--;
                print_indentation(depth);
                printf("}\n");
                break;
            case YAML_SEQUENCE_START_EVENT:
                print_indentation(depth);
                printf("[\n");
                depth++;
                is_key = 0;

                if (current_state == STATE_EXPECTING_SYMLINKS_SEQUENCE) {
                    current_state = STATE_IN_SYMLINKS_SEQUENCE;
                    symlinks_count = 0;
                }
                break;
            case YAML_SEQUENCE_END_EVENT:
                depth--;
                print_indentation(depth);
                printf("]\n");

                if (current_state == STATE_IN_SYMLINKS_SEQUENCE) {
                    printf("  Total symlink pairs found: %d\n", symlinks_count);
                    current_state = STATE_IN_MODULE_MAPPING;
                }
                break;
            case YAML_SCALAR_EVENT:
                print_indentation(depth);
                if (is_key) {
                    printf("%s\n", (char *)event.data.scalar.value);
                    is_key = 0;

                    if (strcmp((char *)event.data.scalar.value, "symlinks") == 0) {
                        current_state = STATE_EXPECTING_SYMLINKS_SEQUENCE;
                    }
                } else {
                    printf("%s\n", (char *)event.data.scalar.value);
                    is_key = 1;
                }
                break;
            default:
                break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    return EXIT_SUCCESS;
}
