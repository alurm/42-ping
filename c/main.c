#include <stdio.h>
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

    struct icmphdr icmp = {
        .type = ICMP_ECHO,

        .code = 0,
        // Fix me: calculate the checksum.
        .checksum = 0,
        .un.echo = {
            // If code = 0, an identifier to aid in matching echos and replies,
            // may be zero.
            .id = 0,
            // If code = 0, a sequence number to aid in matching echos and
            // replies, may be zero.
            .sequence = 0,
        },
    };
}
