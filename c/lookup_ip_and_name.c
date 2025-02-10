#include "lib.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

struct ip_and_name lookup_ip_and_name(char *name) {
    struct ip_and_name result = { 0 };

    struct addrinfo *getaddrinfo_result;

    int error = getaddrinfo(
            name,
            0,
            &(struct addrinfo){
                .ai_family = AF_INET,
                .ai_flags = AI_CANONNAME,
            },
            &getaddrinfo_result
    );

    if (error != 0) {
        fprintf(stderr, "ping: getaddrinfo failed: %s\n", gai_strerror(error));
        exit(1);
    }

    must(
        getaddrinfo_result->ai_addr->sa_family == AF_INET,
        "expected the result from getaddrinfo to be an IPv4 address"
    );

    result.ip = *(typeof(result.ip) *)getaddrinfo_result->ai_addr;

    if (getaddrinfo_result->ai_canonname)
        result.name = try(
            0,
            "strdup failed",
            strdup(getaddrinfo_result->ai_canonname)
        );

    freeaddrinfo(getaddrinfo_result);

    return result;
}
