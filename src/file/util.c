#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "../log/util.h"

int printFile(char* fp) {
    FILE* fh = fopen(fp, "r");
    if (!fh) {
        logError("Could not open file at \'%s\'", fp);
        return 1;
    }

    int c;
    while ((c = fgetc(fh)) != EOF) {
        putchar(c);
    }
    fclose(fh);

    return 0;
}

int expandEnvVars(char** outputPathPtr, const char* inputPath) {
    if (!inputPath || !outputPathPtr) {
        logError("Invalid arguments passed to expandEnvVars (NULL pointer detected).");
        return -1;
    }

    const size_t maxPathLen = 4096;
    char buffer[maxPathLen];
    memset(buffer, 0, maxPathLen);
    const char* p = inputPath;
    char* currentOutput = buffer;
    size_t remainingLen = maxPathLen - 1;

    if (*p == '~') {
        char* homeDir = getenv("HOME");
        if (homeDir) {
            size_t homeLen = strlen(homeDir);
            if (homeLen >= remainingLen) {
                logError("Path expansion buffer overflow! Expanded path too long.");
                return -1; 
            }

            strncpy(currentOutput, homeDir, remainingLen);
            currentOutput += homeLen;
            remainingLen -= homeLen;
        }
        p++;
    }

    while (*p != '\0') {
        if (remainingLen == 0) {
            logError("Path expansion buffer overflow! Path too long.");
            return -1;
        }

        if (*p == '$') {
            const char* varStart = p + 1;
            const char* varEnd = varStart;

            while (*varEnd && (*varEnd != '/') && (*varEnd != '.') && (*varEnd != '-')) {
                varEnd++;
            }

            size_t varNameLen = varEnd - varStart;
            if (varNameLen > 0) {
                char varName[varNameLen + 1];
                strncpy(varName, varStart, varNameLen);
                varName[varNameLen] = '\0';

                char* varValue = getenv(varName);
                if (varValue) {
                    size_t valueLen = strlen(varValue);
                    if (valueLen >= remainingLen) {
                        logError("Path expansion buffer overflow! Path too long after expanding variable '%s'.", varName);
                        return -1;
                    }

                    strncpy(currentOutput, varValue, remainingLen);
                    currentOutput += valueLen;
                    remainingLen -= valueLen;
                    p = varEnd;
                    continue;
                }
            }
        }

        *currentOutput++ = *p;
        remainingLen--;

        p++;
    }

    *currentOutput = '\0';

    char* newPath = strdup(buffer);
    if (!newPath) {
        logError("Memory allocation failed when expanding path '%s'.", inputPath);
        return -1;
    }

    *outputPathPtr = newPath;

    return 0;
}
