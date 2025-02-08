#include "sys/socket.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <unistd.h>

#define try(error_code, error_message, expression) ({ \
    typeof(expression) result = (expression); \
    if (result == error_code) { \
        perror("ping: " error_message); \
        exit(1); \
    } \
    result; \
})

#ifndef release
#define wip(...)
#endif

void must(_Bool boolean, char *error_message) {
    if (!boolean) {
        fprintf(stderr, "ping: %s\n", error_message);
        exit(1);
    }
}

// "The checksum is the 16-bit ones's complement of the one's complement sum of the ICMP message starting with the ICMP Type."
// https://en.wikipedia.org/wiki/Internet_checksum.
// https://datatracker.ietf.org/doc/html/rfc1071 "Computing the Internet checksum".
uint16_t calculate_icmp_checksum(struct icmphdr *packet_as_icmp_header, size_t size) {
    uint32_t sum = 0;

    char *packet = (void *)packet_as_icmp_header;

    for (size_t i = 0; i < size; i += 2) {
        // This one is more significant (larger) than the next octet, because the network octet order is big endian.
        // (https://datatracker.ietf.org/doc/html/rfc1700 "Assigned numbers".
        sum += packet[i] << 8;

        if (i + 1 < size) sum += packet[i + 1];

        // Looping since adding a carry may produce yet another carry.
        while (1) {
            uint16_t carry = sum >> 16;

            if (carry == 0) break;

            // Only keep the 16 least significant bits.
            uint32_t mask = ~(~0u << 16);

            uint16_t without_carry = sum & mask;

            sum = carry + without_carry;
        }
    }

    // Convert the sum to big endian and invert the bits ("one's complement").
    return ~htons(sum);
}

// We're using SOCK_RAW and not SOCK_DGRAM.
// SOCK_DGRAM is appealing since it makes the kernel compute checksums of messages for us.
// But SOCK_DGRAM doesn't provide the IP headers on recv, which are needed for verbose output.
// What's not so fun about SOCK_RAW is that it needs CAP_NET_RAW.
//
// Technically, we can fallback to using SOCK_DGRAM when SOCK_RAW doesn't work.
// This is what inetutils' ping does, but this is currently not implemented in this tool.
//
// We could set the IP_HDRINCL socket option here.
// This would make sent packets have to contain the IP header.
// But it seems like TTL is the only option we need, for which we have IP_TTL.
// So we don't currently set it.
//
// References: ip(7), raw(7).
static int open_raw_socket(void) {
    return try(
        -1,
        "opening the raw socket failed",
        socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    );
}

wip(getaddrinfo)
struct sockaddr_in lookup_ip(void) {
    struct sockaddr_in localhost_ip = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl((127 << 8 * 3) + 0 + 0 + 1),
    };

    return localhost_ip;
}

enum {
    // 21 is an arbitrary value.
    echo_request_data_size = wip() 21,
    // Note: "All hosts must be prepared to accept datagrams of up to 576 octets".
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol".
    echo_request_icmp_packet_size = echo_request_data_size + sizeof(struct icmphdr),
};

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

int main(void) {
    int raw_socket = open_raw_socket();

    {
        // Filter out all message types except ICMP echo replies.
        // Not sure how other types of messages should be handled by ping.
        // Filtering them out seems line an okay solution.

        struct icmp_filter filter = {
            .data = ~(1 << ICMP_ECHOREPLY),
        };

        try(
            -1,
            "failed to set the socket option to only leave ICMP echo replies",
            setsockopt(raw_socket, IPPROTO_RAW, ICMP_FILTER, &filter, sizeof(struct icmp_filter))
        );
    }

    struct sockaddr_in ip = lookup_ip(); wip(bind?)

    wip() printf("PING [...] ([...]): [...] data bytes\n");

    wip(should be endless and have a sleep period)
    for (size_t i = 1; i <= 5; i++) {
        {
            struct icmphdr *request = make_new_echo_request_packet();

            wip(should errors be handled more gracefully?)
            try(
                -1,
                "failed to send an echo request",
                sendto(raw_socket, request, echo_request_icmp_packet_size, 0, (void *)&ip, sizeof ip)
            );
            
            free(request);
        }

        {
            // The length of a datagram is up to 65535 octets.
            // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol"
            char reply_buffer[1 << 16];

            wip(should errors be handled more gracefully?)
            try(
                -1,
                "failed to receive a packet",
                wip(recv) recv(raw_socket, reply_buffer, sizeof reply_buffer, 0)
            );

            struct iphdr *ip = (void *)reply_buffer;
            struct icmphdr *icmp = (void *)(reply_buffer + sizeof(struct iphdr));

            wip() must(icmp->type == ICMP_ECHOREPLY, "expected an ICMP echo reply");

            // "Internet Header Length is the length of the internet header in 32 bit words".
            // https://datatracker.ietf.org/doc/html/rfc791 "Internet protocol"
            size_t icmp_size = ntohs(ip->tot_len) - ip->ihl * 4;

            wip() printf(
                "%zu bytes from [...]: icmp_seq=%d ttl=%d time=[...] ms\n",
                icmp_size,
                ntohs(icmp->un.echo.sequence),
                ip->ttl
            );

            wip(dump the IPv4 header if -v is given)
        }
    }

    wip() printf(
        "--- [...] ping statistics ---\n"
        "[...] packets transmitted, [...] packets received, [...]%% packet loss\n"
        "round-trip min/avg/max/stddev = [...]/[...]/[...]/[...] ms"
    );
}
