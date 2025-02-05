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

int main(void) {
    int raw_socket = try(
        "Couldn't open the raw socket",
        socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    );

    char packet[sizeof(struct icmphdr) + 20] = { 0 };

    struct icmphdr icmp = {
        .type = ICMP_ECHO,
        .code = 0,
        // Fix me: calculate the correct checksum.
        .checksum = 0,
        .un.echo = {
            // "If code = 0, an identifier to aid in matching echos and replies,
            // may be zero."
            .id = 0,
            // "If code = 0, a sequence number to aid in matching echos and
            // replies, may be zero."
            .sequence = 0,
        },
    };

    memcpy(packet, &icmp, sizeof icmp);

    struct sockaddr_in localhost = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl((127 << 8 * 3) + 0 + 0 + 1),
    };

    try("Send failed", sendto(raw_socket, packet, sizeof packet, 0, (struct sockaddr *)&localhost, sizeof localhost));
}
