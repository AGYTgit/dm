#ifndef PARSE_YAML_DUNGEON_H
#define PARSE_YAML_DUNGEON_H

#include "../../structs/dungeonConf.h"

config parseDungeonConf(const char* filePath);

int getConfig(config* conf, const char* configPath);

int printAppConf(config conf);

#endif // PARSE_YAML_DUNGEON_H
