#include <stdio.h>
#include <string.h>
#include <toml.h>

#include "utils/log.h"
#include "parse/flags.c"
#include "parse/commands.c"

int main(int argc, char* argv[]) {
    parsedArgs args = parseArgs(argc, argv);

    if (args.muteFlag) {
        setLogLevel(LOG_LEVEL_MUTE);
    } else if (args.quietFlag) {
        setLogLevel(LOG_LEVEL_ERROR);
    } else if (args.verboseFlag) {
        setLogLevel(LOG_LEVEL_DEBUG);
    } else {
        setLogLevel(LOG_LEVEL_INFO);
    }

    logError("Hello from DotMod!");
    logWarning("Hello from DotMod!");
    logInfo("Hello from DotMod!");
    logDebug("Hello from DotMod!");

    if (args.versionFlag || args.command == COMMAND_GET_VERSION) {
        logWarning("NYI: prints the version");
        return EXIT_SUCCESS;
    }

    if (args.helpFlag) {
        logWarning("NYI: prints the help page");
        return EXIT_SUCCESS;
    }

    return 0;
}
