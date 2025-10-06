#include "commands.h"
#include <string.h>
#include <stdlib.h>

static void setCommandError(command* cmd, commandErrorType type, const char* value) {
    cmd->error.type = type;

    free(cmd->error.value);
    if (value) {
        cmd->error.value = strdup(value);
    } else {
        cmd->error.value = NULL;
    }
}

command parseCommand(int argc, char* argv[], int* optind) {
    command cmd = {0};

    if ((*optind) >= argc) {
        return cmd;
    }

    char* cmdTypeStr = argv[(*optind)];
    (*optind)++;

    if (!strcmp(cmdTypeStr, "init")) {
        cmd.command = COMMAND_INIT;
        if ((*optind) < argc) {
            cmd.value = strdup(argv[(*optind)]);
            (*optind)++;
        } else {
            cmd.value = "default";
        }
    } else if (!strcmp(cmdTypeStr, "commit")) {
        cmd.command = COMMAND_COMMIT;
    } else if (!strcmp(cmdTypeStr, "apply")) {
        cmd.command = COMMAND_APPLY;
    } else if (!strcmp(cmdTypeStr, "backup")) {
        cmd.command = COMMAND_BACKUP;
    } else if (!strcmp(cmdTypeStr, "restore")) {
        cmd.command = COMMAND_RESTORE;
    } else if (!strcmp(cmdTypeStr, "status")) {
        cmd.command = COMMAND_STATUS;
    } else if (!strcmp(cmdTypeStr, "check")) {
        cmd.command = COMMAND_CHECK;
    } else if (!strcmp(cmdTypeStr, "help")) {
        cmd.command = COMMAND_HELP;
    } else if (!strcmp(cmdTypeStr, "load")) {
        cmd.command = COMMAND_LOAD;
        if ((*optind) < argc) {
            cmd.value = argv[(*optind)];
            (*optind)++;
        } else {
            setCommandError(&cmd, COMMAND_ERROR_MISSING_VALUE, NULL);
            return cmd;
        }
    } else if (!strcmp(cmdTypeStr, "uload")) {
        cmd.command = COMMAND_ULOAD;
        if ((*optind) < argc) {
            cmd.value = argv[(*optind)];
            (*optind)++;
        } else {
            setCommandError(&cmd, COMMAND_ERROR_MISSING_VALUE, NULL);
        return cmd;
        }
    }
    // NOTE: rewrite this to use a recursive function for parsing
    else if (!strcmp(cmdTypeStr, "version")) {
        cmd.command = COMMAND_VERSION;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            else if (!strcmp(argv[*optind], "get")) { cmd.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            else { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            (*optind)++;
        } else {
            cmd.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "path")) {
        cmd.command = COMMAND_PATH;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                cmd.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    cmd.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    setCommandError(&cmd, COMMAND_ERROR_MISSING_VALUE, NULL);
                    return cmd;
                }
            }
            else if (!strcmp(argv[*optind], "get")) { cmd.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            else { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            (*optind)++;
        } else {
            cmd.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "profile")) {
        cmd.command = COMMAND_PROFILE;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                cmd.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    cmd.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    setCommandError(&cmd, COMMAND_ERROR_MISSING_VALUE, NULL);
                    return cmd;
                }
            }
            else if (!strcmp(argv[*optind], "get")) { cmd.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { cmd.action = ACTION_LIST; } 
            else { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            (*optind)++;
        } else {
            cmd.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "theme")) {
        cmd.command = COMMAND_THEME;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) {
                cmd.action = ACTION_SET;
                if ((*optind) + 1 < argc) {
                    cmd.value = argv[(*optind) + 1];
                    (*optind)++;
                } else {
                    setCommandError(&cmd, COMMAND_ERROR_MISSING_VALUE, NULL);
                    return cmd;
                }
            }
            else if (!strcmp(argv[*optind], "get")) { cmd.action = ACTION_GET; }
            else if (!strcmp(argv[*optind], "list")) { cmd.action = ACTION_LIST; }
            else { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            (*optind)++;
        } else {
            cmd.action = ACTION_GET;
        }
    } else if (!strcmp(cmdTypeStr, "module")) {
        cmd.command = COMMAND_MODULE;
        if ((*optind) < argc) {
            if (!strcmp(argv[*optind], "set")) { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            else if (!strcmp(argv[*optind], "get")) { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            else if (!strcmp(argv[*optind], "list")) { cmd.action = ACTION_LIST; }
            else { setCommandError(&cmd, COMMAND_ERROR_INVALID_ACTION, NULL); return cmd; }
            (*optind)++;
        } else {
            cmd.action = ACTION_LIST;
        }
    }
    else {
        setCommandError(&cmd, COMMAND_ERROR_UNKNOWN_COMMAND, NULL);
        return cmd;
    }

    if ((*optind) < argc) {
        setCommandError(&cmd, COMMAND_ERROR_UNEXPECTED_TRAILING_ARGS, NULL);
        return cmd;
    }

    return cmd;
}
