#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>

#include "../log/util.h"

#include "core.h"

static int emit_key_plain_value(yaml_emitter_t* emitter, const char* key, const char* value) {
    yaml_event_t event;
    if (!yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)key, strlen(key), 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(emitter, &event) ||
        !yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)value, strlen(value), 1, 1, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(emitter, &event)) {
        logError("Failed to emit plain key-value pair: '%s' = '%s'", key, value);
        return 0;
    }
    return 1;
}

static int emit_key_quoted_value(yaml_emitter_t* emitter, const char* key, const char* value) {
    yaml_event_t event;
    if (!yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)key, strlen(key), 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(emitter, &event) ||
        !yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)value, strlen(value), 0, 1, YAML_DOUBLE_QUOTED_SCALAR_STYLE) ||
        !yaml_emitter_emit(emitter, &event)) {
        logError("Failed to emit quoted key-value pair: '%s' = '%s'", key, value);
        return 0;
    }
    return 1;
}

int emitAppConf(const config* conf, const char* filePath) {
    FILE *file_out;
    yaml_emitter_t emitter;
    yaml_event_t event;
    int success = 0;

    logDebug("Starting emitAppConf to file: %s", filePath);

    file_out = fopen(filePath, "wb");
    if (!file_out) {
        logError("failed to open output file");
        return -1;
    }

    if (!yaml_emitter_initialize(&emitter)) {
        logError("failed to initialize emitter");
        fclose(file_out);
        return -1;
    }

    yaml_emitter_set_output_file(&emitter, file_out);
    yaml_emitter_set_indent(&emitter, 2);
    yaml_emitter_set_canonical(&emitter, 0);

    // Emitting stream and document start events
    if (!yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 1) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit document start events");
        goto cleanup;
    }

    // Top-level 'config' mapping
    if (!yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)"config", 6, 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit top-level 'config' mapping");
        goto cleanup;
    }

    // 'app' sub-mapping
    if (!yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)"app", 3, 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !emit_key_quoted_value(&emitter, "name", conf->app.name) ||
        !emit_key_quoted_value(&emitter, "version", conf->app.version) ||
        !emit_key_quoted_value(&emitter, "description", conf->app.description) ||
        !yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit 'app' section");
        goto cleanup;
    }

    // 'paths' sub-mapping
    if (!yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)"paths", 5, 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !emit_key_quoted_value(&emitter, "repo", conf->paths.repo) ||
        !emit_key_quoted_value(&emitter, "template", conf->paths.template) ||
        !emit_key_quoted_value(&emitter, "backup", conf->paths.backup) ||
        !emit_key_quoted_value(&emitter, "log", conf->paths.log) ||
        !yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit 'paths' section");
        goto cleanup;
    }

    // 'behavior' sub-mapping
    if (!yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t *)"behavior", 8, 1, 0, YAML_PLAIN_SCALAR_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !emit_key_plain_value(&emitter, "autoGit", conf->behavior.autoGit ? "true" : "false") ||
        !emit_key_plain_value(&emitter, "autoApply", conf->behavior.autoApply ? "true" : "false") ||
        !emit_key_plain_value(&emitter, "promptForConfirmation", conf->behavior.promptForConfirmation ? "true" : "false") ||
        !yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit 'behavior' section");
        goto cleanup;
    }

    // End all remaining mappings and events
    if (!yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_document_end_event_initialize(&event, 1) ||
        !yaml_emitter_emit(&emitter, &event) ||
        !yaml_stream_end_event_initialize(&event) ||
        !yaml_emitter_emit(&emitter, &event)) {
        logError("Failed to emit final events");
        goto cleanup;
    }

    success = 1;

cleanup:
    logDebug("Cleaning up emitter and file...");
    yaml_emitter_flush(&emitter);

    if (success) {
        logDebug("emitAppConf finished successfully.");
    } else {
        logError("YAML emission failed");
    }

    yaml_emitter_delete(&emitter);
    fclose(file_out);

    return success ? 0 : -1;
}
