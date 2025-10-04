#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils/log.h"
#include "utils/file.h"

#include "parse/flags.h"
#include "parse/commands.h"

#include "parse/yaml/module.h"
#include "parse/yaml/config.h"

#include "structs/dis.h"
#include "dispatcher/dis.h"

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

int main(int argc, char* argv[]) {
    extraPaths exPaths;
    expandEnvVars(&exPaths.help, "$HOME/projects/dm/dm/src/resources/help.txt");
    expandEnvVars(&exPaths.conf, "$HOME/projects/dm/dm/conf/config.yaml");

    config conf;
    if (getConfig(&conf, exPaths.conf) != 0) {
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
        return printFile(exPaths.help);
    }

    if (flags.version) {
        logDebug("flag: version");
        logBlank("%s\n", conf.app.version);
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
        .exPaths = exPaths
    };
    funcDis[cmd.command][cmd.action](&data);

    freeDisArgs(&data);

    return 0;
}
