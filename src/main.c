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
        logWarning("NYI: prints the help page");
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

    logDebug("optind: %d", optind);

    parsedCommands commands = parseCommands(argc, argv, &optind);

    logError("Hello from DotMod!");
    logWarning("Hello from DotMod!");
    logInfo("Hello from DotMod!");
    logDebug("Hello from DotMod!");

    if (flags.helpFlag) {
        logWarning("NYI: prints the help page");
        return EXIT_SUCCESS;
    }

    if (flags.versionFlag) {
        logWarning("NYI: prints the version");
        return EXIT_SUCCESS;
    }

    return 0;
}
