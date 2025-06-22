#ifndef PARSE_FLAGS_H
#define PARSE_FLAGS_H

#include <stdbool.h>

/**
 * @brief Structure to hold all parsed flags
 */
typedef struct {
    char* customPath; // Path for --custom
    bool customFlag;
    bool muteFlag;
    bool quietFlag;
    bool verboseFlag;
    bool forceFlag;
    bool helpFlag;
    bool versionFlag;
    bool parsingError;
} parsedFlags;

/**
 * @brief Parses flags
 */
parsedFlags parseFlags(int argc, char* argv[], int* optindStart);

#endif // PARSE_FLAGS_H
