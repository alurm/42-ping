#include "library.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

struct sockaddr_in lookup_ip(char *name) {
    struct addrinfo *getaddrinfo_result;

    int error = getaddrinfo(
            name,
            0,
            &(struct addrinfo){ .ai_family = AF_INET },
            &getaddrinfo_result
    );

    if (error != 0) {
        fprintf(stderr, "ping: getaddrinfo failed: %s\n", gai_strerror(error));
        exit(1);
    }

    struct sockaddr *ip_ptr = getaddrinfo_result->ai_addr;

    must(
        ip_ptr->sa_family == AF_INET,
        "expected the result from getaddrinfo to be an IPv4 address"
    );

    struct sockaddr_in ip = *(typeof(ip) *)ip_ptr;

    freeaddrinfo(getaddrinfo_result);

    return ip;
}
