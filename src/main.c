#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

#include "utils/log.h"
#include "parse/flags.h"
#include "parse/commands.h"
#include "parse/module.h"

int loadModule(char* filepath) {
    module mod = parseModule(filepath);

    if (mod.error.type) {
        printf("ERROR [%d]: %s\n", mod.error.type, mod.error.value ? mod.error.value : "");
        return 1;
    }

    printf("\n");

    printf("name: %s\n", mod.name);
    printf("version: %s\n", mod.version);
    printf("path: \'%s\'\n", mod.path);

    printf("level: %d\n", mod.conf.level);

    printf("%s", (mod.deps.module.count + mod.deps.pacman.count + mod.deps.yay.count) > 0 ? "deps:\n" : "");
    printf("%s", mod.deps.module.count > 0 ? "    module:\n" : "");
    for (size_t i = 0; i < mod.deps.module.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.module.value[i]);
    }
    printf("%s", mod.deps.pacman.count > 0 ? "    pacman:\n" : "");
    for (size_t i = 0; i < mod.deps.pacman.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.pacman.value[i]);
    }
    printf("%s", mod.deps.yay.count > 0 ? "    yay:\n" : "");
    for (size_t i = 0; i < mod.deps.yay.count; ++i) {
        printf("        [%d]: %s\n", i, mod.deps.yay.value[i]);
    }

    printf("%s", mod.links.count > 0 ? "links:\n" : "");
    for (size_t i = 0; i < mod.links.count; ++i) {
        printf("    source: %s -> target: %s\n", mod.links.source[i], mod.links.target[i]);
    }

    printf("\n");
}

int parseFC(int argc, char* argv[]) {
    int optind = 1;
    parsedFlags flag = parseFlags(argc, argv, &optind);

    if (flag.parsingError) {
        logWarning("NYI: flag parsing error");
        return EXIT_FAILURE;
    }

    if (flag.mute) {
        setLogLevel(LOG_LEVEL_MUTE);
    } else if (flag.verbose) {
        setLogLevel(LOG_LEVEL_DEBUG);
    } else if (flag.quiet) {
        setLogLevel(LOG_LEVEL_ERROR);
    } else {
        setLogLevel(LOG_LEVEL_INFO);
    }
    logDebug("Set logLevel to %s", getLogLevelStr());

    if (flag.help) {
        logWarning("NYI: flag help");
        return EXIT_SUCCESS;
    }

    if (flag.version) {
        logWarning("NYI: flag version");
        return EXIT_SUCCESS;
    }

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
    else if (command.cmd == CMD_LOAD)    { logInfo("command: load %s", command.value);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s%s%s", "../templates/default/modules/", command.value, "/module.yaml");
        loadModule(buffer);
    }
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

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (parseFC(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    logInfo("Hello from DotMod!");

    return 0;
}
