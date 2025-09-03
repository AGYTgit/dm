#ifndef DISPATCHER_COMMAND_H
#define DISPATCHER_COMMAND_H

#include "../structs/dis.h"
#include "../structs/command.h"

typedef int (*functionDispatcher)(disArgs* data);

int cmdNone(disArgs* data);
int cmdInit(disArgs* data);
int cmdCommit(disArgs* data);
int cmdApply(disArgs* data);
int cmdBackup(disArgs* data);
int cmdRestore(disArgs* data);
int cmdStatus(disArgs* data);
int cmdCheck(disArgs* data);
int cmdHelp(disArgs* data);
int cmdLoad(disArgs* data);
int cmdUload(disArgs* data);

int cmdVersionGet(disArgs* data);

int cmdPathSet(disArgs* data);
int cmdPathGet(disArgs* data);

int cmdProfileSet(disArgs* data);
int cmdProfileGet(disArgs* data);
int cmdProfileList(disArgs* data);

int cmdThemeSet(disArgs* data);
int cmdThemeGet(disArgs* data);
int cmdThemeList(disArgs* data);

int cmdModuleList(disArgs* data);

extern functionDispatcher funcDis[COMMAND_COUNT][ACTION_COUNT];

#endif // DISPATCHER_COMMAND_H
