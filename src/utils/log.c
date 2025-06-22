#include "log.h"
#include <stdio.h>    // For fprintf, stderr, va_list, vsnprintf
#include <stdarg.h>   // For va_list, va_start, va_end
#include <time.h>     // For time, localtime, strftime (for timestamps)

// Static global variable to hold the current logging threshold
static logLevel currentLogLevel = LOG_LEVEL_INFO; // Default to INFO

/**
 * @brief Internal helper function to format and print log messages
 * @param level The level of the current message
 * @param prefix The string prefix for the log message (e.g., "ERROR", "INFO")
 * @param format The format string
 * @param args The va_list of arguments
 */
static void logMessage(logLevel level, const char* prefix, const char* format, va_list args) {
    if (currentLogLevel >= level) {
        if (currentLogLevel == LOG_LEVEL_DEBUG) {
            char timeBuffer[32];
            time_t timer;
            struct tm* tmInfo;

            time(&timer);
            tmInfo = localtime(&timer);

            // Format timestamp as YYYY-MM-DD HH:MM:SS
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tmInfo);

            fprintf(stderr, "[%s] %s: ", timeBuffer, prefix);
        } else {
            fprintf(stderr, "%s: ", prefix);
        }

        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
    }
}

void setLogLevel(logLevel level) {
    currentLogLevel = level;
}

char* getLogLevel() {
    switch (currentLogLevel) {
    case LOG_LEVEL_MUTE:
        return "MUTE";
        break;
    case LOG_LEVEL_ERROR:
        return "ERROR";
        break;
    case LOG_LEVEL_WARNING:
        return "WARNING";
        break;
    case LOG_LEVEL_INFO:
        return "INFO";
        break;
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
        break;
    }
}

void logError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_ERROR, "ERROR", format, args);
    va_end(args);
}

void logWarning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_WARNING, "WARNING", format, args);
    va_end(args);
}

void logInfo(const char* format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_INFO, "INFO", format, args);
    va_end(args);
}

void logDebug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_DEBUG, "DEBUG", format, args);
    va_end(args);
}
