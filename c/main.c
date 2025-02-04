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
}
