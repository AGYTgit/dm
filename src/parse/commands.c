#include "commands.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Helper function to create and return a parsedCommand in an error state
 */
static parsedCommand createErrorCommand(errorType error) {
    parsedCommand errorCommand = {
        .cmd = CMD_NONE,
        .action = ACTION_NONE,
        .value = NULL,
        .parsingError = true,
        .errorType = error
    };
    return errorCommand;
}

/**
 * @brief Parses the main command, its optional action and optional value
 * This function is called after global flags have been parsed by parseFlags
 */
parsedCommand parseCommand(int argc, char* argv[], int* optind) {
    parsedCommand commands = {
        .cmd = CMD_NONE,
        .action = ACTION_NONE,
        .value = NULL,
        .parsingError = false,
        .errorType = CMD_NONE
    };

    if ((*optind) >= argc) {
        return commands;
    }

    char* cmdTypeStr = argv[(*optind)];

    // Identify command
    if (!strcmp(cmdTypeStr, "init")) {
        commands.cmd = CMD_INIT;
        (*optind)++;
        if ((*optind) < argc) {
            commands.value = argv[(*optind)];
        } else {
            commands.value = "";
        }
    } else if (!strcmp(cmdTypeStr, "commit")) {
        commands.cmd = CMD_COMMIT;
    } else if (!strcmp(cmdTypeStr, "backup")) {
        commands.cmd = CMD_BACKUP;
    } else if (!strcmp(cmdTypeStr, "restore")) {
        commands.cmd = CMD_RESTORE;
    } else if (!strcmp(cmdTypeStr, "status")) {
        commands.cmd = CMD_STATUS;
    } else if (!strcmp(cmdTypeStr, "check")) {
        commands.cmd = CMD_CHECK;
    } else if (!strcmp(cmdTypeStr, "help")) {
        commands.cmd = CMD_HELP;
    } else if (!strcmp(cmdTypeStr, "load")) {
        commands.cmd = CMD_LOAD;
        (*optind)++;
        if ((*optind) < argc) {
            commands.value = argv[(*optind)];
        } else {
            return createErrorCommand(ERROR_MISSING_VALUE);
        }
    } else if (!strcmp(cmdTypeStr, "uload")) {
        commands.cmd = CMD_ULOAD;
        (*optind)++;
        if ((*optind) < argc) {
            commands.value = argv[(*optind)];
        } else {
            return createErrorCommand(ERROR_MISSING_VALUE);
        }
    }
    // Commands that can take either of set/get/list subcommands
    else if (!strcmp(cmdTypeStr, "version")) {
        commands.cmd = CMD_VERSION;
    } else if (!strcmp(cmdTypeStr, "path")) {
        commands.cmd = CMD_PATH;
    } else if (!strcmp(cmdTypeStr, "profile")) {
        commands.cmd = CMD_PROFILE;
    } else if (!strcmp(cmdTypeStr, "theme")) {
        commands.cmd = CMD_THEME;
    } else if (!strcmp(cmdTypeStr, "modules")) {
        commands.cmd = CMD_MODULES;
    } else if (!strcmp(cmdTypeStr, "profiles")) {
        commands.cmd = CMD_PROFILES;
    }
    else {
        return createErrorCommand(ERROR_UNKNOWN_CMD);
    }

    if (commands.cmd == CMD_VERSION ||
        commands.cmd == CMD_PATH ||
        commands.cmd == CMD_PROFILE ||
        commands.cmd == CMD_THEME) {
        if ((*optind) == argc) {
            commands.action = ACTION_GET;
        } else if ((*optind) < argc) {
            (*optind)++;

            if (!strcmp(argv[*optind], "set")) {
            }
            if (!strcmp(argv[*optind], "get")) {
            }
            if (!strcmp(argv[*optind], "list")) {
            }
        }
    }

    if ((*optind) < argc) {
        return createErrorCommand(ERROR_UNEXPECTED_TRAILING_ARGS);
    }

    return commands;
}
