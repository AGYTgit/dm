#include "commands.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Helper function to create and return a parsedCommand in an error state
 */
static parsedCommand createErrorCommand(errorType err) {
    parsedCommand errorCommand = {
        .cmd = CMD_NONE,
        .action = ACTION_NONE,
        .value = NULL,
        .error = err
    };
    return errorCommand;
}

/**
 * @brief Parses the main command, its optional action and optional value
 * This function is called after global flags have been parsed by parseFlags
 */
parsedCommand parseCommand(int argc, char* argv[], int* optind) {
    parsedCommand command = {
        .cmd = CMD_NONE,
        .action = ACTION_NONE,
        .value = NULL,
        .error = CMD_NONE
    };

    if ((*optind) >= argc) {
        return command;
    }

    char* cmdTypeStr = argv[(*optind)];
    (*optind)++;

    // Identify command
    if (!strcmp(cmdTypeStr, "init")) {
        command.cmd = CMD_INIT;
        if ((*optind) < argc) {
            command.value = argv[(*optind)];
            (*optind)++;
        } else {
            command.value = "default";
        }
    } else if (!strcmp(cmdTypeStr, "commit")) {
        command.cmd = CMD_COMMIT;
    } else if (!strcmp(cmdTypeStr, "backup")) {
        command.cmd = CMD_BACKUP;
    } else if (!strcmp(cmdTypeStr, "restore")) {
        command.cmd = CMD_RESTORE;
    } else if (!strcmp(cmdTypeStr, "status")) {
        command.cmd = CMD_STATUS;
    } else if (!strcmp(cmdTypeStr, "check")) {
        command.cmd = CMD_CHECK;
    } else if (!strcmp(cmdTypeStr, "help")) {
        command.cmd = CMD_HELP;
    } else if (!strcmp(cmdTypeStr, "load")) {
        command.cmd = CMD_LOAD;
        if ((*optind) < argc) {
            command.value = argv[(*optind)];
            (*optind)++;
        } else {
            return createErrorCommand(ERROR_MISSING_VALUE);
        }
    } else if (!strcmp(cmdTypeStr, "uload")) {
        command.cmd = CMD_ULOAD;
        if ((*optind) < argc) {
            command.value = argv[(*optind)];
            (*optind)++;
        } else {
            return createErrorCommand(ERROR_MISSING_VALUE);
        }
    }
    // commands that can take either of set/get/list subcommands
    // NOTE: rewrite this to use a recursive function for parsing
    else if (!strcmp(cmdTypeStr, "version")) {
        command.cmd = CMD_VERSION;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) { return createErrorCommand(ERROR_INVALID_ACTION); }
            else if (!strcmp(argv[*optind], "get")) { command.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { return createErrorCommand(ERROR_INVALID_ACTION); }
            else { return createErrorCommand(ERROR_INVALID_ACTION); }
            (*optind)++;
        } else {
            command.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "path")) {
        command.cmd = CMD_PATH;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                command.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    command.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    return createErrorCommand(ERROR_MISSING_VALUE);
                }
            }
            else if (!strcmp(argv[*optind], "get")) { command.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { return createErrorCommand(ERROR_INVALID_ACTION); }
            else { return createErrorCommand(ERROR_INVALID_ACTION); }
            (*optind)++;
        } else {
            command.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "profile")) {
        command.cmd = CMD_PROFILE;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                command.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    command.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    return createErrorCommand(ERROR_MISSING_VALUE);
                }
            }
            else if (!strcmp(argv[*optind], "get")) { command.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { command.action = ACTION_LIST; } 
            else { return createErrorCommand(ERROR_INVALID_ACTION); }
            (*optind)++;
        } else {
            command.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "theme")) {
        command.cmd = CMD_THEME;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                command.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    command.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    return createErrorCommand(ERROR_MISSING_VALUE);
                }
            }
            else if (!strcmp(argv[*optind], "get")) { command.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { command.action = ACTION_LIST; } 
            else { return createErrorCommand(ERROR_INVALID_ACTION); }
            (*optind)++;
        } else {
            command.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "module")) {
        command.cmd = CMD_MODULE;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) { return createErrorCommand(ERROR_INVALID_ACTION); }
            else if (!strcmp(argv[*optind], "get")) { return createErrorCommand(ERROR_INVALID_ACTION); }
            else if (!strcmp(argv[*optind], "list")) { command.action = ACTION_LIST; } 
            else { return createErrorCommand(ERROR_INVALID_ACTION); }
            (*optind)++;
        } else {
            command.action = ACTION_LIST;
        }
    }
    else {
        return createErrorCommand(ERROR_UNKNOWN_CMD);
    }

    if ((*optind) < argc) {
        return createErrorCommand(ERROR_UNEXPECTED_TRAILING_ARGS);
    }

    return command;
}
