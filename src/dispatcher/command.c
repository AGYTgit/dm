#include <stdlib.h>
#include <stdio.h>

#include "../utils/log.h"

#include "../parse/flags.h"
#include "../parse/commands.h"

#include "../parse/yaml/module.h"
#include "../parse/yaml/config.h"


char* configPath = "/home/agyt/projects/dm/dm/conf/config.yaml";
char* helpPath = "/home/agyt/projects/dm/dm/src/resources/help.txt";

commandDispatcher cmdDis[COMMAND_COUNT][ACTION_COUNT] = {
    // ENUM           ACTION_NONE     ACTION_SET      ACTION_GET      ACTION_LIST
    [COMMAND_NONE]    = { cmdNone,        cmdNone,        cmdNone,        cmdNone },

    [COMMAND_INIT]    = { cmdInit,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_COMMIT]  = { cmdCommit,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_APPLY]   = { cmdApply,       cmdNone,        cmdNone,        cmdNone },
    [COMMAND_BACKUP]  = { cmdBackup,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_RESTORE] = { cmdRestore,     cmdNone,        cmdNone,        cmdNone },
    [COMMAND_STATUS]  = { cmdStatus,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_CHECK]   = { cmdCheck,       cmdNone,        cmdNone,        cmdNone },
    [COMMAND_HELP]    = { cmdHelp,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_LOAD]    = { cmdLoad,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_UNLOAD]  = { cmdUnload,      cmdNone,        cmdNone,        cmdNone },

    [COMMAND_VERSION] = { cmdNone,        cmdNone,        cmdVersionGet,  cmdNone },
    [COMMAND_PATH]    = { cmdNone,        cmdPathSet,     cmdPathGet,     cmdNone },
    [COMMAND_PROFILE] = { cmdNone,        cmdProfileSet,  cmdProfileGet,  cmdProfileList },
    [COMMAND_THEME]   = { cmdNone,        cmdThemeSet,    cmdThemeGet,    cmdThemeList },
    [COMMAND_MODULE]  = { cmdNone,        cmdNone,        cmdNone,        cmdModuleList },
};

int cmdNone(void* data) {
    (void)data;
    return 0;
}

int cmdInit(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: init %s", cmd->value);
    return 0;
}

int cmdCommit(void* data) {
    (void)data;
    logWarning("NYI: command: commit");
    return 0;
}

int cmdApply(void* data) {
    (void)data;
    logWarning("NYI: command: apply");
    return 0;
}

int cmdBackup(void* data) {
    (void)data;
    logWarning("NYI: command: backup");
    return 0;
}

int cmdRestore(void* data) {
    (void)data;
    logWarning("NYI: command: restore");
    return 0;
}

int cmdStatus(void* data) {
    (void)data;
    logBlank("--- %s ---\n", conf.app.version);
    logWarning("NYI: command: status");
    printAppConf(conf);
    freeConfig(&conf);
    return 0;
}

int cmdCheck(void* data) {
    (void)data;
    logWarning("NYI: command: check");
    return 0;
}

int cmdHelp(void* data) {
    (void)data;
    logDebug("command: help");
    return printHelpPage(helpPath);
}

int cmdLoad(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: load %s", cmd->value);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "/home/agyt/projects/dm/dm/templates/default/%s/module.yaml", cmd->value);
    module mod = parseModule(buffer);
    printModuleConf(mod);
    freeModule(&mod);
    return 0;
}

int cmdUnload(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: uload %s", cmd->value);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "/home/agyt/projects/dm/dm/templates/default/%s/module.yaml", cmd->value);
    module mod = parseModule(buffer);
    if (mod.error.type) {
        logError("[%d] %s", mod.error.type, mod.error.value ? mod.error.value : "");
        return -1;
    }
    for (size_t i = 0; i < mod.commands.load.count; ++i) {
        logBlank("command on load [%zu]: %s\n", i, mod.commands.load.value[i]);
        int returnCode = system(mod.commands.load.value[i]);
        if (returnCode != 0) {
            logBlank("command failed with return code: %d\n", returnCode);
        }
    }
    for (size_t i = 0; i < mod.commands.uload.count; ++i) {
        logBlank("command on uload [%zu]: %s\n", i, mod.commands.uload.value[i]);
        int returnCode = system(mod.commands.uload.value[i]);
        if (returnCode != 0) {
            logBlank("command failed with return code: %d\n", returnCode);
        }
    }
    freeModule(&mod);
    return 0;
}

int cmdVersionGet(void* data) {
    (void)data;
    logDebug("command: version get");
    logBlank("%s\n", conf.app.version);
    freeConfig(&conf);
    return 0;
}

int cmdPathSet(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: path set %s", cmd->value);
    return 0;
}

int cmdPathGet(void* data) {
    (void)data;
    logDebug("command: path get");
    logBlank("%s\n", conf.paths.repo);
    freeConfig(&conf);
    return 0;
}

int cmdProfileSet(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: profile set %s", cmd->value);
    return 0;
}

int cmdProfileGet(void* data) {
    (void)data;
    logWarning("NYI: command: profile get");
    return 0;
}

int cmdProfileList(void* data) {
    (void)data;
    logWarning("NYI: command: profile list");
    return 0;
}

int cmdThemeSet(void* data) {
    parsedCommand* cmd = (parsedCommand*)data;
    logWarning("NYI: command: theme set %s", cmd->value);
    return 0;
}

int cmdThemeGet(void* data) {
    (void)data;
    logWarning("NYI: command: theme get");
    return 0;
}

int cmdThemeList(void* data) {
    (void)data;
    logWarning("NYI: command: theme list");
    return 0;
}

int cmdModuleList(void* data) {
    (void)data;
    logWarning("NYI: command: module list");
    return 0;
}
