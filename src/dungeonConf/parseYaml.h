#ifndef PARSE_YAML_DUNGEON_H
#define PARSE_YAML_DUNGEON_H

#include "core.h"

dungeonConf parseDungeonConf(const char* filePath);

int getDungeonConf(dungeonConf* conf, const char* filePath);

#endif // PARSE_YAML_DUNGEON_H
