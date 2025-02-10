#include "library.h"

#include <stdlib.h>
#include <stdio.h>

void bug(char *error_message) {
    fprintf(stderr, "ping: bug: %s\n", error_message);
    exit(1);
}
