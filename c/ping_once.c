#include "library.h"

#include <linux/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

static void send_ping(int socket, struct sockaddr_in ip) {
    auto request = make_new_echo_request_packet();

    try(
        -1,
        "sending packet",
        sendto(
            socket,
            request,
            icmp_echo_packet_size,
            0,
            (void *)&ip,
            sizeof ip
        )
    );

    free(request);
}

struct ping_result ping_once(int socket, struct sockaddr_in ip, bool verbose) {
    send_ping(socket, ip);

    char buffer[max_packet_size];
    auto pong = receive_pong(socket, buffer);

    switch (pong.status)
    case pong_no_response: if (1) {
        return (struct ping_result){ .have_received_reply = false };
    } else
    case pong_ok:
    case pong_time_exceeded: if (1) {
        printf(
            "%zu bytes from %s: ",
            pong.icmp_size,
            inet_ntoa((struct in_addr){ .s_addr = pong.ip->saddr })
        );
        if (pong.status == pong_ok) {
            printf(
                "icmp_seq=%d ttl=%d time=%.3f ms \n",
                ntohs(pong.icmp->un.echo.sequence),
                pong.ip->ttl,
                pong.delta
            );
            return (struct ping_result){ .have_received_reply = true, .delta = pong.delta };
        } else
        {
            printf("Time to live exceeded\n");
            // The headers printed are of the received packet.
            if (verbose) ping_dump_packet(pong.ip, pong.icmp, pong.icmp_size);
            return (struct ping_result){ .have_received_reply = false };
        }
    }
}
