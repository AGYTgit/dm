#ifndef PARSE_YAML_CONFIG_H
#define PARSE_YAML_CONFIG_H

#include "../../structs/config.h"

config parseConfig(const char* filePath);

void freeConfig(config* conf);

#endif // PARSE_YAML_CONFIG_H
