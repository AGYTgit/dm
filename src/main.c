#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils/log.h"

#include "parse/flags.h"
#include "parse/commands.h"

#include "parse/yaml/module.h"
#include "parse/yaml/config.h"

#include "dispatcher/dis.h"

char* configPath = "/home/agyt/projects/dm/dm/conf/config.yaml";
char* helpPath = "/home/agyt/projects/dm/dm/src/resources/help.txt";

int printHelpPage(char* fp) {
    FILE* fh = fopen(fp, "r");
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
        logBlank("ERROR [%d]: %s\n", conf.error.type, conf.error.value ? conf.error.value : "");
        return 1;
    }

    logBlank("\n");

    logBlank("name:        %s\n", conf.app.name);
    logBlank("version:     %s\n", conf.app.version);
    logBlank("description: %s\n", conf.app.description);
    logBlank("repo:   %s\n", conf.paths.repo);
    logBlank("backup: %s\n", conf.paths.backup);
    logBlank("log:    %s\n", conf.paths.log);
    logBlank("autoGit:               %s\n", conf.behavior.autoGit ? "true" : "false");
    logBlank("promptForConfirmation: %s\n", conf.behavior.promptForConfirmation ? "true" : "false");

    logBlank("\n");

    return 0;
}

int getFlags(int argc, char* argv[], flags* flags, int* optindPtr) {
    *flags = parseFlags(argc, argv, optindPtr);

    if (flags->error.type != 0) {
        logError("[%d]: flag parsing failed %s", flags->error.type, flags->error.value ? flags->error.value : "");
        return 1;
    }

    return 0;
}


int getCommand(int argc, char* argv[], command* cmd, int* optindPtr) {
    *cmd = parseCommand(argc, argv, optindPtr);

    if (cmd->error.type != 0) {
        logError("[%d] while parsing commands %s", cmd->error.type, cmd->error.value ? cmd->error.value : "");
        return 1;
    }

    return 0;
}

int getConfig(config* conf, const char* configPath) {
    *conf = parseConfig(configPath);

    if (conf->error.type) {
        logError("[%d] while parsing config %s", conf->error.type, conf->error.value ? conf->error.value : "");
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    config conf;
    if (getConfig(&conf, configPath) != 0) {
        return 1;
    }

    int optind = 1;
    flags flags;
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
        return printHelpPage(helpPath);
    }

    if (flags.version) {
        logDebug("flag: version");
        logBlank("%s\n", conf.app.version);
        freeConfig(&conf);
        return 0;
    }

    command cmd;
    if (getCommand(argc, argv, &cmd, &optind) != 0) {
        return 0;
    }

    disArgs data = {
        .flags = flags,
        .cmd = cmd,
        .conf = conf,
    };
    data.conf.paths.template = "/home/agyt/projects/dm/dm/templates/default";
    funcDis[cmd.command][cmd.action](&data);

    // switch (cmd.command) {
    //     case COMMAND_INIT:
    //         logWarning("NYI: command: init %s", cmd.value);
    //         break;
    //     case COMMAND_COMMIT:
    //         logWarning("NYI: command: commit");
    //         break;
    //     case COMMAND_APPLY:
    //         logWarning("NYI: command: apply");
    //         break;
    //     case COMMAND_BACKUP:
    //         logWarning("NYI: command: backup");
    //         break;
    //     case COMMAND_RESTORE:
    //         logWarning("NYI: command: restore");
    //         break;
    //     case COMMAND_STATUS:
    //         logBlank("--- %s ---\n", conf.app.version);
    //         logWarning("NYI: command: status");

    //         // temporary just to test config parsing
    //         printAppConf(conf);
    //         freeConfig(&conf);
    //         return 0;

    //         break;
    //     case COMMAND_CHECK:
    //         logWarning("NYI: command: check");
    //         break;
    //     case COMMAND_HELP:
    //         logDebug("command: help");
    //         return printHelpPage(helpPath);
    //     case COMMAND_LOAD: {
    //         logWarning("NYI: command: load %s", cmd.value);

    //         // temporary just to test module parsing
    //         char buffer[128];
    //         snprintf(buffer, sizeof(buffer), "../templates/default/%s/module.yaml", cmd.value);
    //         module mod = parseModule(buffer);
    //         printModuleConf(mod);
    //         freeModule(&mod);
    //         return 0;

    //         break;
    //     }
    //     case COMMAND_ULOAD: {
    //         logWarning("NYI: command: uload %s", cmd.value);

    //         //temporary to test module's command execution
    //         char buffer[128];
    //         snprintf(buffer, sizeof(buffer), "../templates/default/%s/module.yaml", cmd.value);
    //         module mod = parseModule(buffer);
    //         if (mod.error.type) {
    //             logError("[%d] %s", mod.error.type, mod.error.value ? mod.error.value : "");
    //             break;
    //         }

    //         for (size_t i = 0; i < mod.commands.load.count; ++i) {
    //             logBlank("command on load [%d]: %s\n", i, mod.commands.load.value[i]);
    //             int returnCode = system(mod.commands.load.value[i]);
    //             if (returnCode != 0) {
    //                 logBlank("command failed with return code: %d\n", returnCode);
    //             }
    //         }
    //         for (size_t i = 0; i < mod.commands.uload.count; ++i) {
    //             logBlank("command on uload [%d]: %s\n", i, mod.commands.uload.value[i]);
    //             int returnCode = system(mod.commands.uload.value[i]);
    //             if (returnCode != 0) {
    //                 logBlank("command failed with return code: %d\n", returnCode);
    //             }
    //         }
    //         freeModule(&mod);
    //         return 0;

    //         break;
    //     }
    //     case COMMAND_VERSION:
    //         switch (cmd.action) {
    //             case ACTION_GET:
    //                 logDebug("command: version get");
    //                 logBlank("%s\n", conf.app.version);
    //                 freeConfig(&conf);
    //                 return 0;
    //         }
    //         break;
    //     case COMMAND_PATH:
    //         switch (cmd.action) {
    //             case ACTION_SET:
    //                 logWarning("NYI: command: path set %s", cmd.value);
    //                 break;
    //             case ACTION_GET:
    //                 logDebug("command: path get");
    //                 logBlank("%s\n", conf.paths.repo);
    //                 freeConfig(&conf);
    //                 return 0;
    //         }
    //         break;
    //     case COMMAND_PROFILE:
    //         switch (cmd.action) {
    //             case ACTION_SET:
    //                 logWarning("NYI: command: profile set %s", cmd.value);
    //                 break;
    //             case ACTION_GET:
    //                 logWarning("NYI: command: profile get");
    //                 break;
    //             case ACTION_LIST:
    //                 logWarning("NYI: command: profile list");
    //                 break;
    //         }
    //         break;
    //     case COMMAND_THEME:
    //         switch (cmd.action) {
    //             case ACTION_SET:
    //                 logWarning("NYI: command: theme set %s", cmd.value);
    //                 break;
    //             case ACTION_GET:
    //                 logWarning("NYI: command: theme get");
    //                 break;
    //             case ACTION_LIST:
    //                 logWarning("NYI: command: theme list");
    //                 break;
    //         }
    //         break;
    //     case COMMAND_MODULE:
    //         switch (cmd.action) {
    //             case ACTION_LIST:
    //                 logWarning("NYI: command: module list");
    //                 break;
    //         }
    //         break;
    // }

    return 0;
}
