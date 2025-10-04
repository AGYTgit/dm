#ifndef UTILS_FILE_H
#define UTILS_FILE_H

int printFile(char* fp);
int expandEnvVars(char** outputPathPtr, const char* inputPath);

#endif // UTILS_FILE_H
