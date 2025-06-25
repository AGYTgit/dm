#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

#include "utils/log.h"
#include "parse/flags.h"
#include "parse/commands.h"

int main(int argc, char* argv[]) {
    int optind = 1;
    parsedFlags flags = parseFlags(argc, argv, &optind);

    if (flags.parsingError) {
        logWarning("NYI: flag parsing error");
        return EXIT_FAILURE;
    }

    if (flags.muteFlag) {
        setLogLevel(LOG_LEVEL_MUTE);
    } else if (flags.verboseFlag) {
        setLogLevel(LOG_LEVEL_DEBUG);
    } else if (flags.quietFlag) {
        setLogLevel(LOG_LEVEL_ERROR);
    } else {
        setLogLevel(LOG_LEVEL_INFO);
    }
    logDebug("Set logLevel to %s", getLogLevel());

    logDebug("parsing flags");
    logDebug("optind: %d", 1);

    if (flags.helpFlag) {
        logWarning("NYI: flag help");
        return EXIT_SUCCESS;
    }

    if (flags.versionFlag) {
        logWarning("NYI: flag version");
        return EXIT_SUCCESS;
    }

    logDebug("parsing command");
    logDebug("optind: %d", optind);
    parsedCommand command = parseCommand(argc, argv, &optind);

    if (command.error != ERROR_NONE) {
        logWarning("NYI: command parsing error %d", command.error);
        return EXIT_FAILURE;
    }

    if      (command.cmd == CMD_INIT)    { logInfo("command: init %s", command.value); }
    else if (command.cmd == CMD_COMMIT)  { logInfo("command: commit"); }
    else if (command.cmd == CMD_BACKUP)  { logInfo("command: backup"); }
    else if (command.cmd == CMD_RESTORE) { logInfo("command: restore"); }
    else if (command.cmd == CMD_STATUS)  { logInfo("command: status"); }
    else if (command.cmd == CMD_CHECK)   { logInfo("command: check"); }
    else if (command.cmd == CMD_HELP)    { logInfo("command: help"); }
    else if (command.cmd == CMD_LOAD)    { logInfo("command: load %s", command.value); }
    else if (command.cmd == CMD_ULOAD)   { logInfo("command: uload %s", command.value); }

    else if   (command.cmd == CMD_VERSION) {
        if      (command.action == ACTION_GET)  { logInfo("command: version get"); }
    } else if (command.cmd == CMD_PATH) {
        if      (command.action == ACTION_SET)  { logInfo("command: path set %s", command.value); }
        if      (command.action == ACTION_GET)  { logInfo("command: path get"); }
    } else if (command.cmd == CMD_PROFILE) {
        if      (command.action == ACTION_SET)  { logInfo("command: profile set %s", command.value); }
        if      (command.action == ACTION_GET)  { logInfo("command: profile get"); }
        if      (command.action == ACTION_LIST) { logInfo("command: profile list"); }
    } else if (command.cmd == CMD_THEME) {
        if      (command.action == ACTION_SET)  { logInfo("command: theme set %s", command.value); }
        if      (command.action == ACTION_GET)  { logInfo("command: theme get"); }
        if      (command.action == ACTION_LIST) { logInfo("command: theme list"); }
    } else if (command.cmd == CMD_MODULE) {
        if      (command.action == ACTION_LIST) { logInfo("command: module list"); }
    }

    logInfo("Hello from DotMod!");

    return 0;
}
