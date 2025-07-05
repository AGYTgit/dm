#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseModule.h"

int main(void) {
    char* fh = "module.yaml";

    module mod = parseModule(fh);

    return 0;
}
