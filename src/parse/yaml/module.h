#ifndef PARSE_YAML_MODULE_H
#define PARSE_YAML_MODULE_H

#include "../../structs/module.h"

module parseModule(const char* filePath);

void freeModule(module* mod);

#endif // PARSE_YAML_MODULE_H
