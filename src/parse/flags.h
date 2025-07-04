#ifndef PARSE_FLAGS_H
#define PARSE_FLAGS_H

#include <stdbool.h>

/**
 * @brief Structure to hold all parsed flags
 */
typedef struct {
    char* customPath; // Path for --custom
    bool custom;
    bool mute;
    bool quiet;
    bool verbose;
    bool force;
    bool help;
    bool version;
    bool parsingError;
} parsedFlags;

/**
 * @brief Parses flags
 */
parsedFlags parseFlags(int argc, char* argv[], int* optind);

#endif // PARSE_FLAGS_H
