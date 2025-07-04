#ifndef LOG_UTILS_H
#define LOG_UTILS_H

typedef enum {
    LOG_LEVEL_MUTE,      // Suppress all output (corresponds to --mute)
    LOG_LEVEL_ERROR,     // Only error messages (corresponds to --quiet)
    LOG_LEVEL_WARNING,   // Warning messages and above
    LOG_LEVEL_INFO,      // Informational messages and above (default)
    LOG_LEVEL_DEBUG      // Debug messages and above (corresponds to --verbose)
} logLevel;

/**
 * @brief Sets the global logging threshold (level)
 * Messages with a level lower than the set threshold will not be printed
 */
void setLogLevel(logLevel level);

/**
 * @brief Returnds log level
 */
char* getLogLevelStr();

/**
 * @brief Logs an error message
 */
void logError(const char* format, ...);

/**
 * @brief Logs a warning message
 */
void logWarning(const char* format, ...);

/**
 * @brief Logs an informational message
 */
void logInfo(const char* format, ...);

/**
 * @brief Logs a debug message
 */
void logDebug(const char* format, ...);

#endif // LOG_UTILS_H
