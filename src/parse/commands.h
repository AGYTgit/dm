#ifndef PARSE_COMMANDS_H
#define PARSE_COMMANDS_H

#include "../structs/command.h"

command parseCommand(int argc, char* argv[], int* optind);

#endif // PARSE_COMMANDS_H
