#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"
#include "file.h"

int printFile(char* fp) {
    FILE* fh = fopen(fp, "r");
    if (!fh) {
        logError("Could not open file at \'%s\'", fp);
        return 1;
    }

    int c;
    while ((c = fgetc(fh)) != EOF) {
        putchar(c);
    }
    fclose(fh);

    return 0;
}
