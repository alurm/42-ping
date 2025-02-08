// IWYU pragma: no_include "bits/socket_type.h"

#include "lib.h"

#include <sys/socket.h>
#include <linux/icmp.h>
#include <netinet/in.h>

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
int open_and_configure_raw_socket(void) {
    int raw_socket = try(
        -1,
        "opening the raw socket failed",
        socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    );
 
    // Filter out all message types except ICMP echo replies.
    // Not sure how other types of messages should be handled by ping.
    // Filtering them out seems acceptable.

    struct icmp_filter filter = {
        .data = ~(1 << ICMP_ECHOREPLY),
    };

    try(
        -1,
        "failed to set the socket option to only leave ICMP echo replies",
        setsockopt(raw_socket, IPPROTO_RAW, ICMP_FILTER, &filter, sizeof(struct icmp_filter))
    );

    return raw_socket;
}
