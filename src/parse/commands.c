#include "flags.h"
#include <getopt.h>

// Define long flags for getopt_long
static struct option longflags[] = {
    {"custom",  required_argument, 0, 'c'},
    {"force",   no_argument,       0, 'f'},
    {"verbose", no_argument,       0, 'V'},
    {"quiet",   no_argument,       0, 'q'},
    {"mute",    no_argument,       0, 'm'},
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'v'},
    {0, 0, 0, 0}
};

parsedFlags parseFlags(int argc, char* argv[], int optind=1) {
    parsedFlags flags = {0}; // Initialize all members to 0/false/NULL

    int opt;
    int longIndex = 0;

    const char* optStr = "c:fVqmhv";

    // Reset optind for multiple calls if needed (though typically only called once)
    optind = 1;

    while ((opt = getopt_long(argc, argv, optStr, longflags, &longIndex)) != -1) {
        switch (opt) {
            case 'c':
                flags.customPath = optarg;
                break;
            case 'f':
                flags.forceFlag = true;
                break;
            case 'V':
                flags.verboseFlag = true;
                break;
            case 'q':
                flags.quietFlag = true;
                break;
            case 'm':
                flags.muteFlag = true;
                break;
            case 'h':
                flags.helpFlag = true;
                break;
            case 'v':
                flags.versionFlag = true;
                break;
            case '?':
                flags.parsingError = true;
                return flags;
        }
    }

    // Process non-option arguments (which should be our command and its argument)
    if (optind < argc) {
        char* commandStr = argv[optind];
        // logDebug removed

        if (!strcmp(commandStr, "init")) {
            flags.command = COMMAND_INIT;
        } else if (!strcmp(commandStr, "commit")) {
            flags.command = COMMAND_COMMIT;
        } else if (!strcmp(commandStr, "backup")) {
            flags.command = COMMAND_BACKUP;
        } else if (!strcmp(commandStr, "restore")) {
            flags.command = COMMAND_RESTORE;
        } else if (!strcmp(commandStr, "status")) {
            flags.command = COMMAND_STATUS;
        } else if (!strcmp(commandStr, "check")) {
            flags.command = COMMAND_CHECK;
        } else if (!strcmp(commandStr, "load")) {
            flags.command = COMMAND_LOAD;
        } else if (!strcmp(commandStr, "uload")) {
            flags.command = COMMAND_ULOAD;
        } else if (!strcmp(commandStr, "set-path")) {
            flags.command = COMMAND_SET_PATH;
        } else if (!strcmp(commandStr, "set-profile")) {
            flags.command = COMMAND_SET_PROFILE;
        } else if (!strcmp(commandStr, "set-theme")) {
            flags.command = COMMAND_SET_THEME;
        } else if (!strcmp(commandStr, "get-path")) {
            flags.command = COMMAND_GET_PATH;
        } else if (!strcmp(commandStr, "get-profile")) {
            flags.command = COMMAND_GET_PROFILE;
        } else if (!strcmp(commandStr, "get-theme")) {
            flags.command = COMMAND_GET_THEME;
        } else if (!strcmp(commandStr, "get-version")) {
            flags.command = COMMAND_GET_VERSION;
        } else if (!strcmp(commandStr, "list-profiles")) {
            flags.command = COMMAND_LIST_PROFILES;
        } else if (!strcmp(commandStr, "list-modules")) {
            flags.command = COMMAND_LIST_MODULES;
        } else if (!strcmp(commandStr, "list-themes")) {
            flags.command = COMMAND_LIST_THEMES;
        } else {
            // Unknown command
            flags.parsingError = true; // Indicate a parsing error
            // Removed fprintf(stderr, ...) and exit(EXIT_FAILURE)
            // You might want to store the unknown command string here if main needs it for a specific error message
            return flags; // Return immediately
        }

        // If there's a subsequent argument, it's the command's argument
        if (optind + 1 < argc) {
            flags.argValue = argv[optind + 1];
            // logDebug removed
        }
    }

    return flags;
}
