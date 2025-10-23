#ifndef PARSE_YAML_MODULE_H
#define PARSE_YAML_MODULE_H

#include "core.h"

module parseModule(const char* filePath);

void freeModule(module* mod);

int printModuleConf(module mod);

#endif // PARSE_YAML_MODULE_H
