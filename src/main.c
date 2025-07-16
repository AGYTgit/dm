#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils/log.h"

#include "parse/flags.h"
#include "parse/commands.h"

#include "parse/yaml/module.h"
#include "parse/yaml/config.h"


int printHelpPage() {
    char* helpPath = "resources/help.txt";
    FILE* fh = fopen(helpPath, "r");
    if (!fh) {
        logError("Could not open help file at \'%s\'", helpPath);
        return 1;
    }

    int c;
    while ((c = fgetc(fh)) != EOF) {
        putchar(c);
    }
    fclose(fh);
    return 0;
}

int printAppConf(config conf) {
    if (conf.error.type) {
        printf("ERROR [%d]: %s\n", conf.error.type, conf.error.value ? conf.error.value : "");
        return 1;
    }

    printf("\n");

    printf("name:        %s\n", conf.app.name);
    printf("version:     %s\n", conf.app.version);
    printf("description: %s\n", conf.app.description);
    printf("repo:   %s\n", conf.paths.repo);
    printf("backup: %s\n", conf.paths.backup);
    printf("log:    %s\n", conf.paths.log);
    printf("autoGit:               %s\n", conf.behavior.autoGit ? "true" : "false");
    printf("promptForConfirmation: %s\n", conf.behavior.promptForConfirmation ? "true" : "false");

    printf("\n");

    return 0;
}

int printModuleConf(module mod) {
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

    return 0;
}

int getFlags(int argc, char* argv[], parsedFlags* flags, int* optindPtr) {
    *flags = parseFlags(argc, argv, optindPtr);

    if (flags->parsingError) {
        logError("flag parsing failed");
        return 1;
    }

    return 0;
}


int getCommand(int argc, char* argv[], parsedCommand* command, int* optindPtr) {
    *command = parseCommand(argc, argv, optindPtr);

    if (command->error != ERROR_NONE) {
        logError("[%d] while parsing commands", command->error);
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int optind = 1;
    parsedFlags flags;
    if (getFlags(argc, argv, &flags, &optind) != 0) {
        return 1;
    }

    if (flags.mute) {
        setLogLevel(LOG_LEVEL_MUTE);
    } else if (flags.verbose) {
        setLogLevel(LOG_LEVEL_DEBUG);
    } else if (flags.quiet) {
        setLogLevel(LOG_LEVEL_ERROR);
    } else {
        setLogLevel(LOG_LEVEL_INFO);
    }
    logDebug("Set logLevel to %s", getLogLevelStr());

    if (flags.help) {
        logDebug("flag: help");
        return printHelpPage();
    }

    if (flags.version) {
        logWarning("NYI: flag version");
        // print version
        return 0;
    }

    parsedCommand command;
    if (getCommand(argc, argv, &command, &optind) != 0) {
        return 0;
    }

    switch (command.cmd) {
        case CMD_INIT:
            logWarning("NYI: command: init %s", command.value);
            break;
        case CMD_COMMIT:
            logWarning("NYI: command: commit");
            break;
        case CMD_APPLY:
            logWarning("NYI: command: apply");
            break;
        case CMD_BACKUP:
            logWarning("NYI: command: backup");
            break;
        case CMD_RESTORE:
            logWarning("NYI: command: restore");
            break;
        case CMD_STATUS:
            logWarning("NYI: command: status");

            // temporary just to test config parsing
            config conf = parseConfig("../conf/config.yaml");
            printAppConf(conf);
            freeConfig(&conf);

            break;
        case CMD_CHECK:
            logWarning("NYI: command: check");
            break;
        case CMD_HELP:
            logDebug("command: help");
            return printHelpPage();
        case CMD_LOAD:
            logWarning("NYI: command: load %s", command.value);

            // temporary just to test module parsing
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "../templates/default/%s/module.yaml", command.value);
            module mod = parseModule(buffer);
            printModuleConf(mod);
            freeModule(&mod);

            break;
        case CMD_ULOAD:
            logWarning("NYI: command: uload %s", command.value);
            break;
        case CMD_VERSION:
            switch (command.action) {
                case ACTION_GET:
                    logWarning("NYI: command: version get");
                    break;
            }
            break;
        case CMD_PATH:
            switch (command.action) {
                case ACTION_SET:
                    logWarning("NYI: command: path set %s", command.value);
                    break;
                case ACTION_GET:
                    logWarning("NYI: command: path get");
                    break;
            }
            break;
        case CMD_PROFILE:
            switch (command.action) {
                case ACTION_SET:
                    logWarning("NYI: command: profile set %s", command.value);
                    break;
                case ACTION_GET:
                    logWarning("NYI: command: profile get");
                    break;
                case ACTION_LIST:
                    logWarning("NYI: command: profile list");
                    break;
            }
            break;
        case CMD_THEME:
            switch (command.action) {
                case ACTION_SET:
                    logWarning("NYI: command: theme set %s", command.value);
                    break;
                case ACTION_GET:
                    logWarning("NYI: command: theme get");
                    break;
                case ACTION_LIST:
                    logWarning("NYI: command: theme list");
                    break;
            }
            break;
        case CMD_MODULE:
            switch (command.action) {
                case ACTION_LIST:
                    logWarning("NYI: command: module list");
                    break;
            }
            break;
    }

    return 0;
}
