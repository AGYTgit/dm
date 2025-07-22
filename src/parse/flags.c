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

flags parseFlags(int argc, char* argv[], int* optindStart) {
    flags flags = {0};

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
            flags.custom = optarg;
            break;
        case 'f':
            flags.force = true;
            break;
        case 'V':
            flags.verbose = true;
            break;
        case 'q':
            flags.quiet = true;
            break;
        case 'm':
            flags.mute = true;
            break;
        case 'h':
            flags.help = true;
            break;
        case 'v':
            flags.version = true;
            break;
        case '?':
            flags.error.type = FLAG_ERROR_UNKNOWN_FLAG;
            return flags;
        }
    }

    *optindStart = optind;
    return flags;
}
