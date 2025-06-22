#include "flags.h"
#include <getopt.h>

// Define long flags for getopt_long
static struct option allFlags[] = {
    {"custom",  required_argument, 0, 'c'},
    {"force",   no_argument,       0, 'f'},
    {"verbose", no_argument,       0, 'V'},
    {"quiet",   no_argument,       0, 'q'},
    {"mute",    no_argument,       0, 'm'},
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'v'},
    {0, 0, 0, 0}
};

parsedFlags parseFlags(int argc, char* argv[], int* optindStart) {
    parsedFlags flags = {0};

    if (*optindStart >= argc) {
        return flags;
    }

    optind = *optindStart;

    int opt;
    int flagsIndex = 0;

    const char* optStr = "c:fVqmhv";

    while ((opt = getopt_long(argc, argv, optStr, allFlags, &flagsIndex)) != -1) {
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

    *optindStart = optind;
    return flags;
}
