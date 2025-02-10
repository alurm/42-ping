#include "library.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <linux/icmp.h>
#include <stdlib.h>

struct icmphdr *make_new_echo_request_packet(uint16_t identifier) {
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
            .id = identifier,
            .sequence = htons(sequence),
        }
    };

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
