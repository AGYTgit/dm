#include <stdio.h>
#include <string.h>
#include <toml.h>

#include "util.h"

int main(int argc, char* argv[]) {
    print_msg("Hello from DotMod!");

    const int size = 100;
    char path[size];
    if (argc >= 2) {
        if (!strcmp(argv[1], "-p")) {
            if (argc < 3) {
                printf("ERROR: path not provided\n");
            } else {
                strcpy(path, argv[2]);
                printf("%s\n", path);
            }
        }
    }


    return 0;
}
