#include "library.h"

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <linux/icmp.h>
#include <stdlib.h>
#include <sys/time.h>

struct icmphdr *make_new_echo_request_packet(void) {
    struct icmphdr *packet = try(
        0,
        "malloc failed",
        malloc(icmp_echo_packet_size)
    );

    *packet = (struct icmphdr){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo = {
            .id = ping_statistics_data.identifier,
            .sequence = htons(ping_statistics_data.current_sequence),
        }
    };

    ping_statistics_data.current_sequence++;

    char data[echo_request_data_size] = {};

    // https://stackoverflow.com/questions/70175164/icmp-timestamps-added-to-ping-echo-requests-in-linux-how-are-they-represented-t.
    // Including the UNIX timestamp of the time of transmission in the first data bytes of the ICMP Echo message is a
    // trick/optimization the original ping by Mike Muuss used to avoid keeping track of it locally.
    gettimeofday((void *)data, 0);
    
    memcpy(
        (char*)packet + sizeof(*packet),
        data,
        echo_request_data_size
    );

    packet->checksum = calculate_icmp_checksum(packet, icmp_echo_packet_size);

    return packet;
}
