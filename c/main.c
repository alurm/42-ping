#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <linux/icmp.h>

#define try(error_message, expression) ({ \
    ssize_t result = (expression); \
    if (result < 0) { \
        perror(error_message); \
        exit(1); \
    } \
    result; \
})

// "The checksum is the 16-bit ones's complement of the one's complement sum of the ICMP message starting with the ICMP Type."
uint16_t calculate_icmp_checksum(char *packet, size_t length) {
    uint32_t sum = 0;

    for (size_t i = 0; i < length; i += 2) {
        // This one is more significant (larger) than the next octet.
        // (Assumming the next octet is present).
        // (The network octet order is big endian: https://datatracker.ietf.org/doc/html/rfc1700).
        sum += packet[i] << 8;

        if (i + 1 < length) sum += packet[i + 1];

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

    // Convert the sum to the network octet order and invert the bits (one's complement).
    return ~htons(sum);
}

int main(void) {
    int raw_socket = try(
        "Opening raw socket failed",
        socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    );

    char packet[sizeof(struct icmphdr) + 21] = {
        [sizeof(struct icmphdr) + 20] = 5,
    };

    struct icmphdr *packet_as_icmp_header = (void *)packet;

    *packet_as_icmp_header = (struct icmphdr){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo = {
            // "If code = 0, an identifier to aid in matching echos and replies, may be zero."
            .id = 0,
            // "If code = 0, a sequence number to aid in matching echos and replies, may be zero."
            .sequence = 0,
        },
    };

    packet_as_icmp_header->checksum = calculate_icmp_checksum(packet, sizeof packet);

    printf("%x\n", packet_as_icmp_header->checksum);

    for (size_t i = 1; i < 5; i++) {
        struct sockaddr_in localhost = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = htonl((127 << 8 * 3) + 0 + 0 + 1),
        };

        // Not necessarily needed.
        try(
            "Setting time to live failed",
            setsockopt(raw_socket, IPPROTO_IP, IP_TTL, &(int){ i }, sizeof(int))
        );

        try(
            "Sending a packet failed",
            sendto(raw_socket, packet, sizeof packet, 0, (struct sockaddr *)&localhost, sizeof localhost)
        );
    }
}
