#include "library.h"

void print_ping_header(char *ip) {
    printf(
        "PING %s (%s): %d data bytes",
        ping_statistics_data.host,
        ip,
        echo_request_data_size
    );

    if (ping_statistics_data.verbose)
        printf(", id 0x%lx = %lu", ping_statistics_data.identifier, ping_statistics_data.identifier);

    printf("\n");

    free(ip);
}
