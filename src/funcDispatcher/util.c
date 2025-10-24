#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "core.h"
#include "util.h"

#include "../log/util.h"
#include "../file/util.h"

#include "../dmConfig/parseYaml.h"
#include "../dmConfig/emitYaml.h"

#include "../dungeonModule/parseYaml.h"

#include "../dungeonConf/parseYaml.h"

#include "../flag/core.h"
#include "../command/core.h"
#include "../dmConfig/core.h"

functionDispatcher funcDis[COMMAND_COUNT][ACTION_COUNT] = {
    // ENUM           ACTION_NONE     ACTION_SET      ACTION_GET      ACTION_LIST
    [COMMAND_NONE]    = { cmdHelp,        cmdNone,        cmdNone,        cmdNone },

    [COMMAND_INIT]    = { cmdInit,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_COMMIT]  = { cmdCommit,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_APPLY]   = { cmdApply,       cmdNone,        cmdNone,        cmdNone },
    [COMMAND_BACKUP]  = { cmdBackup,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_RESTORE] = { cmdRestore,     cmdNone,        cmdNone,        cmdNone },
    [COMMAND_STATUS]  = { cmdStatus,      cmdNone,        cmdNone,        cmdNone },
    [COMMAND_CHECK]   = { cmdCheck,       cmdNone,        cmdNone,        cmdNone },
    [COMMAND_HELP]    = { cmdHelp,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_LOAD]    = { cmdLoad,        cmdNone,        cmdNone,        cmdNone },
    [COMMAND_ULOAD]   = { cmdUload,       cmdNone,        cmdNone,        cmdNone },

    [COMMAND_VERSION] = { cmdNone,        cmdNone,        cmdVersionGet,  cmdNone },
    [COMMAND_PATH]    = { cmdNone,        cmdPathSet,     cmdPathGet,     cmdNone },
    [COMMAND_PROFILE] = { cmdNone,        cmdProfileSet,  cmdProfileGet,  cmdProfileList },
    [COMMAND_THEME]   = { cmdNone,        cmdThemeSet,    cmdThemeGet,    cmdThemeList },
    [COMMAND_MODULE]  = { cmdNone,        cmdNone,        cmdNone,        cmdModuleList },
};

int cmdNone(disArgs* data) {
    logWarning("possible errer in function dispatcher, cmdNone should never happen");
    (void) data;
    return 0;
}

int cmdInit(disArgs* data) {
    logDebug("command: init \"%s\"", data->cmd.value);

    struct stat st;
    if (stat(data->cmd.value, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            logError("\"%s\" is a directory", data->cmd.value);
            return -1;
        } else {
            logError("\"%s\" is a file", data->cmd.value);
            return -1;
        }
    }
    if (errno != ENOENT) {
        logError("checking destination path");
        return -1;
    }

    char command[] = "cp -r ";
    size_t buffSize = strlen(command) + strlen(data->conf.paths.template) + strlen(data->cmd.value) + 6;
    char buffer[buffSize];
    snprintf(buffer, sizeof(buffer), "%s\"%s\" \"%s\"", command, data->conf.paths.template, data->cmd.value);

    logDebug("executing: '%s'", buffer);
    system(buffer);

    return 0;
}

int cmdCommit(disArgs* data) {
    logWarning("NYI: command: commit");
    (void) data;
    return 0;
}

int cmdApply(disArgs* data) {
    logWarning("NYI: command: apply");

    dungeonConf conf = {0};
    if (getDungeonConf(&conf, "/home/agyt/projects/dm/dm/conf/current.yaml")) {
        freeDungeonConf(&conf);
        return 1;
    }

    logBlank("profile: %s\n", conf.profile);
    logBlank("theme: %s\n", conf.theme);
    logBlank("modules:\n");
    for (size_t i = 0; i < conf.moduleCount; ++i) {
        logBlank("    %s %d\n", conf.modules[i].name, conf.modules[i].state);
        if (conf.modules[i].state == 1) {
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "%s/modules/%s/module.yaml", data->conf.paths.repo, conf.modules[i].name);
            module mod = parseModule(buffer);
            printModuleConf(mod);
            freeModule(&mod);
        }
    }

    freeDungeonConf(&conf);

    return 0;
}

int cmdBackup(disArgs* data) {
    logWarning("NYI: command: backup");
    (void) data;
    return 0;
}

int cmdRestore(disArgs* data) {
    logWarning("NYI: command: restore");
    (void) data;
    return 0;
}

int cmdStatus(disArgs* data) {
    logWarning("NYI: command: status");
    printAppConf(data->conf);
    return 0;
}

int cmdCheck(disArgs* data) {
    logWarning("NYI: command: check");
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s/conf/config(emit).yaml", data->conf.paths.repo);
    emitAppConf(&(data->conf), buffer);
    return 0;
}

int cmdHelp(disArgs* data) {
    logDebug("command: help");
    printFile(data->exPaths.help);
    return 0;
}

int cmdLoad(disArgs* data) {
    logWarning("NYI: command: load %s", data->cmd.value);
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s/modules/%s/module.yaml", data->conf.paths.repo, data->cmd.value);
    module mod = parseModule(buffer);
    printModuleConf(mod);
    return 0;
}

int cmdUload(disArgs* data) {
    logWarning("NYI: command: uload %s", data->cmd.value);
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "/home/agyt/projects/dm/dm/templates/default/%s/module.yaml", data->cmd.value);
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
    return 0;
}

int cmdVersionGet(disArgs* data) {
    logDebug("command: version get");
    logBlank("%s\n", data->conf.app.version);
    return 0;
}

int cmdPathSet(disArgs* data) {
    logWarning("NYI: command: path set %s", data->cmd.value);
    (void) data;
    return 0;
}

int cmdPathGet(disArgs* data) {
    logDebug("command: path get");
    logBlank("%s\n", data->conf.paths.repo);
    return 0;
}

int cmdProfileSet(disArgs* data) {
    logWarning("NYI: command: profile set %s", data->cmd.value);
    return 0;
}

int cmdProfileGet(disArgs* data) {
    logWarning("NYI: command: profile get");
    (void) data;
    return 0;
}

int cmdProfileList(disArgs* data) {
    logWarning("NYI: command: profile list");
    (void) data;
    return 0;
}

int cmdThemeSet(disArgs* data) {
    logWarning("NYI: command: theme set %s", data->cmd.value);
    return 0;
}

int cmdThemeGet(disArgs* data) {
    logWarning("NYI: command: theme get");
    (void) data;
    return 0;
}

int cmdThemeList(disArgs* data) {
    logWarning("NYI: command: theme list");
    (void) data;
    return 0;
}

int cmdModuleList(disArgs* data) {
    logWarning("NYI: command: module list");
    (void) data;
    return 0;
}
