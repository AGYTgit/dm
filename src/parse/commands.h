#ifndef PARSE_COMMANDS_H
#define PARSE_COMMANDS_H

#include <stdbool.h>

typedef enum {
    CMD_NONE,
    // Standalone commands
    CMD_INIT,
    CMD_COMMIT,
    CMD_BACKUP,
    CMD_RESTORE,
    CMD_STATUS,
    CMD_CHECK,
    CMD_HELP,
    CMD_LOAD,
    CMD_ULOAD,
    // Commands that can be combined with action commands
    CMD_VERSION,
    CMD_PATH,
    CMD_PROFILE,
    CMD_THEME,
    CMD_MODULE,
    // Helper: Represents the number of defined commands
    CMD_COUNT
} cmdType;

typedef enum {
    ACTION_NONE  = 0,
    ACTION_SET   = 1 << 0,
    ACTION_GET   = 1 << 1,
    ACTION_LIST  = 1 << 2
} actionType;

typedef enum {
    ERROR_NONE,
    ERROR_UNKNOWN_CMD,
    ERROR_MISSING_VALUE,
    ERROR_UNEXPECTED_TRAILING_ARGS,
    ERROR_INVALID_ACTION
} errorType;

typedef struct {
    cmdType    cmd;
    actionType action;
    char*      value;
    errorType  error;
} parsedCommand;

/**
 * @brief Parses the main command and optional action/arguments
 * after global flags (if any) have already been parsed
 */
parsedCommand parseCommand(int argc, char* argv[], int* optind);

/**
 * @brief Retrieves the bitmask of allowed actions
 */
int getAllowedActions(cmdType cmd);

/**
 * @brief Checks if a specific action is allowed for a given command
 */
bool isActionAllowed(cmdType cmd, actionType action);

#endif // PARSE_COMMANDS_H
