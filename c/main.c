#include "lib.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    struct program_options options = set_program_options(argc, argv);

    int raw_socket = open_and_configure_raw_socket(options);

    struct ip_and_name ip_and_name = lookup_ip_and_name(options.host);

    char *ip_string_static = inet_ntoa(ip_and_name.ip.sin_addr);

    char *ip_string = try(0, "strdup failed", strdup(ip_string_static));

    printf(
        "PING %s (%s): %d data bytes",
        options.host,
        ip_string,
        echo_request_data_size
    );

    if (options.verbose)
        printf(", id 0x%x = %u", options.identifier, options.identifier);

    printf("\n");

    wip(should be endless and have a sleep period)
    wip(sigint)
    size_t packets_transmitted = 0;
    for (; packets_transmitted < 5; packets_transmitted++) {
        ping_once(
            raw_socket,
            ip_and_name.ip,
            options
        );
    }

    wip(
        packets received, ...,
        ip netns (not related)
    )
    printf(
        "--- %s ping statistics ---\n"
        "%zu packets transmitted, [...] packets received, [...]%% packet loss\n"
        "round-trip min/avg/max/stddev = [...]/[...]/[...]/[...] ms\n",
        options.host,
        packets_transmitted
    );

    free(ip_string);
    free(ip_and_name.name);
}
