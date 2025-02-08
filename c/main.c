#include "lib.h"

#include <netinet/in.h>
#include <stdio.h>

int main(void) {
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
