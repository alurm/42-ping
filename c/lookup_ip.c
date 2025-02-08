#include "lib.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

wip(getaddrinfo)
struct sockaddr_in lookup_ip(void) {
    struct sockaddr_in localhost_ip = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl((127 << 8 * 3) + 0 + 0 + 1),
    };

    return localhost_ip;
}
