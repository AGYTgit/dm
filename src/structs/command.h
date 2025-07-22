#ifndef STRUCT_COMMAND_H
#define STRUCT_COMMAND_H

typedef enum {
    COMMAND_ERROR_NONE,
    COMMAND_ERROR_UNKNOWN_COMMAND,
    COMMAND_ERROR_MISSING_VALUE,
    COMMAND_ERROR_UNEXPECTED_TRAILING_ARGS,
    COMMAND_ERROR_INVALID_ACTION
} commandErrorType;

typedef struct {
    commandErrorType type;
    char* value;
} commandError;

typedef enum {
    COMMAND_NONE,
    // Standalone commands
    COMMAND_INIT,
    COMMAND_COMMIT,
    COMMAND_APPLY,
    COMMAND_BACKUP,
    COMMAND_RESTORE,
    COMMAND_STATUS,
    COMMAND_CHECK,
    COMMAND_HELP,
    COMMAND_LOAD,
    COMMAND_ULOAD,
    // Commands that can be combined with action commands
    COMMAND_VERSION,
    COMMAND_PATH,
    COMMAND_PROFILE,
    COMMAND_THEME,
    COMMAND_MODULE,
    // Helper: Represents the number of defined commands
    COMMAND_COUNT
} commandType;

typedef enum {
    ACTION_NONE,
    ACTION_SET,
    ACTION_GET,
    ACTION_LIST,
    // Helper: Represents the number of defined actions
    ACTION_COUNT
} commandActionType;

typedef struct {
    commandType        command;
    commandActionType  action;
    char*              value;
    commandError       error;
} command;

#endif // STRUCT_COMMAND_H
