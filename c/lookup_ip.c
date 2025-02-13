#include "library.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

struct ip_lookup_result lookup_ip(char *name) {
    struct addrinfo *getaddrinfo_result;

    int error = getaddrinfo(
            name,
            0,
            &(struct addrinfo){ .ai_family = AF_INET },
            &getaddrinfo_result
    );

    must(error == 0, "unknown host");

    struct sockaddr *ip_ptr = getaddrinfo_result->ai_addr;

    must(
        ip_ptr->sa_family == AF_INET,
        "expected the result from getaddrinfo to be an IPv4 address"
    );

    auto ip = *(struct sockaddr_in *)ip_ptr;

    freeaddrinfo(getaddrinfo_result);

    char *string_static = inet_ntoa(ip.sin_addr);

    char *string = try(0, "strdup failed", strdup(string_static));

    return (struct ip_lookup_result){
        .ip = ip,
        .string = string,
    };
}
