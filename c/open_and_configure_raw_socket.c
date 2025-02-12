// IWYU pragma: no_include "bits/socket_type.h"

#include "library.h"

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
int open_and_configure_raw_socket(struct program_options options) {
    int raw_socket = try(
        -1,
        "opening the raw socket failed",
        socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    );

    if (options.have_time_to_live)
        try(
            -1,
            "setting the time to live failed",
            setsockopt(raw_socket, IPPROTO_IP, IP_TTL, &options.time_to_live, sizeof(options.time_to_live))
        );
    
    struct icmp_filter filter = {
        .data =
            ~((1 << ICMP_ECHOREPLY) | (1 << ICMP_TIME_EXCEEDED))
        ,
    };

    try(
        -1,
        "failed to set the socket option to only leave ICMP echo replies",
        setsockopt(raw_socket, IPPROTO_RAW, ICMP_FILTER, &filter, sizeof(struct icmp_filter))
    );

    struct timeval receive_timeout = {
        .tv_sec = receive_timeout_seconds,
    };

    try(
        -1,
        "failed to set the socket option to set a receive timeout",
        setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, &receive_timeout, sizeof receive_timeout)
    );

    return raw_socket;
}
