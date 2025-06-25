#ifndef PARSE_COMMANDS_H
#define PARSE_COMMANDS_H

#include <stdbool.h>

typedef enum {
    CMD_NONE,
    // Standalone Commands
    CMD_INIT,
    CMD_COMMIT,
    CMD_BACKUP,
    CMD_RESTORE,
    CMD_STATUS,
    CMD_CHECK,
    CMD_HELP,
    CMD_LOAD,
    CMD_ULOAD,
    // Commands that *can* take either <set>, <get> or <list>
    CMD_VERSION,
    CMD_PATH,
    CMD_PROFILE,
    CMD_THEME,
    CMD_MODULE,
} cmdType;

typedef enum {
    ACTION_NONE,
    ACTION_SET,
    ACTION_GET,
    ACTION_LIST
} actionType;

typedef enum {
    ERROR_NONE,
    ERROR_UNKNOWN_CMD, // For unidentifiable commands
    ERROR_MISSING_VALUE,  // Missing value for command that requires it
    ERROR_UNEXPECTED_TRAILING_ARGS,
    ERROR_INVALID_ACTION
} errorType;

typedef struct {
    cmdType cmd;
    actionType action;
    char* value;
    errorType error;
} parsedCommand;

/**
 * @brief Parses the main command and its optional action/arguments
 * This function is called after global flags have been parsed by parseFlags
 */
parsedCommand parseCommand(int argc, char* argv[], int* optind);

#endif // PARSE_COMMANDS_H
