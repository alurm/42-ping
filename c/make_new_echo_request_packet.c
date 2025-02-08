#include "lib.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <linux/icmp.h>
#include <stdlib.h>

struct icmphdr *make_new_echo_request_packet(void) {
    static uint16_t sequence;

    sequence++;

    struct icmphdr *packet = try(
        0,
        "malloc failed",
        malloc(echo_request_icmp_packet_size)
    );

    *packet = (typeof(*packet)){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo = {
            .id = htons(getpid() % (1 << 16)),
            .sequence = htons(sequence),
        }
    };

    wip(maybe we can add a --ttl flag)
    // try(
    //     -1,
    //     "setting the time to live failed",
    //     setsockopt(raw_socket, IPPROTO_IP, IP_TTL, &(int) { ... }, sizeof(int))
    // );

    wip(consider including the current time in the data)
    char data[echo_request_data_size] = {
        // Just a value used for testing.
        [echo_request_data_size - 1] = 5
    };

    memcpy(
        (char*)packet + sizeof(*packet),
        data,
        echo_request_data_size
    );

    packet->checksum = calculate_icmp_checksum(packet, echo_request_icmp_packet_size);

    return packet;
}
