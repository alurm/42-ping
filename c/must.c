#include <stdio.h>
#include <stdlib.h>

#include "library.h"

void must(_Bool boolean, char *error_message) {
    if (!boolean) {
        fprintf(stderr, "ping: %s\n", error_message);
        exit(1);
    }
}
