#include "library.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    struct program_options options = set_program_options(argc, argv);

    int raw_socket = open_and_configure_raw_socket(options);

    struct sockaddr_in ip = lookup_ip(options.host);

    char *ip_string_static = inet_ntoa(ip.sin_addr);

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

    struct timeval *times = {};

    size_t packets_transmitted = 0;
    size_t packets_received = 0;

    wip(sigint)
    wip(interval?)
    for (; packets_transmitted < 3; packets_transmitted++) {
        struct ping_result result = ping_once(
            raw_socket,
            ip,
            options
        );

        if (result.have_received_reply) {
            push(times, packets_received, result.time);
        }

        printf("packets received: %zu\n", packets_received);
    }

    for (size_t i = 0; i < packets_received; i++)
        printf("%ld\n", times[i].tv_sec);

    free(times);

    size_t packet_loss = (1 - ((double)packets_received / (double)packets_transmitted)) * 100;

    printf(
        "--- %s ping statistics ---\n"
        "%zu packets transmitted, %zu packets received, %zu%% packet loss\n"
        "round-trip min/avg/max/stddev = [...]/[...]/[...]/[...] ms\n",
        options.host,
        packets_transmitted,
        packets_received,
        packet_loss
    );

    free(ip_string);
}
