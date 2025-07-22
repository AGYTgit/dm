#ifndef DISPATCHER_COMMAND_H
#define DISPATCHER_COMMAND_H

typedef int (*commandDispatcher)(void* data);

int cmdNone(void* data);
int cmdInit(void* data);
int cmdCommit(void* data);
int cmdApply(void* data);
int cmdBackup(void* data);
int cmdRestore(void* data);
int cmdStatus(void* data);
int cmdCheck(void* data);
int cmdHelp(void* data);
int cmdLoad(void* data);
int cmdUnload(void* data);

int cmdVersionGet(void* data);

int cmdPathSet(void* data);
int cmdPathGet(void* data);

int cmdProfileSet(void* data);
int cmdProfileGet(void* data);
int cmdProfileList(void* data);

int cmdThemeSet(void* data);
int cmdThemeGet(void* data);
int cmdThemeList(void* data);

int cmdModuleList(void* data);

commandDispatcher cmdDis[COMMAND_COUNT][ACTION_COUNT];

#endif // DISPATCHER_COMMAND_H
