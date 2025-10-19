#ifndef PARSE_YAML_CONFIG_H
#define PARSE_YAML_CONFIG_H

#include "core.h"

config parseConfig(const char* filePath);

int getConfig(config* conf, const char* configPath);

int printAppConf(config conf);

#endif // PARSE_YAML_CONFIG_H
