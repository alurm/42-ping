#include "lib.h"

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h> // For strcmp.

int main(int argc, char **argv) {
    struct program_options options = parse_program_options(argc, argv);
    (void)options wip();

    int raw_socket = open_and_configure_raw_socket();

    struct sockaddr_in ip = lookup_ip(); wip(bind?)

    wip() printf("PING [...] ([...]): [...] data bytes\n");

    wip(should be endless and have a sleep period)
    for (size_t i = 1; i <= 5; i++) ping_once(
        raw_socket,
        ip
    );

    wip() printf(
        "--- [...] ping statistics ---\n"
        "[...] packets transmitted, [...] packets received, [...]%% packet loss\n"
        "round-trip min/avg/max/stddev = [...]/[...]/[...]/[...] ms"
    );
}
