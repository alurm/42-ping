#include "library.h"

#include <linux/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

struct ping_result ping_once(int raw_socket, struct sockaddr_in ip, struct program_options options) {
    {
        struct icmphdr *request = make_new_echo_request_packet(options.identifier);

        wip(should errors be handled more gracefully?)
        try(
            -1,
            "failed to send an echo request",
            sendto(
                raw_socket,
                request,
                echo_request_icmp_packet_size,
                0,
                (void *)&ip,
                sizeof ip
            )
        );

        free(request);
    }

    {
        // The length of a datagram is up to 65535 octets.
        // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol"
        char reply_buffer[max_packet_size];

        ssize_t recv_result = recv(raw_socket, reply_buffer, sizeof reply_buffer, 0);

        if (recv_result == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return (struct ping_result) { .have_received_reply = false };
            else {
                perror("ping: failed to receive a packet");
                exit(1);
            }
        }

        struct iphdr *ip = (void *)reply_buffer;

        if (ntohs(ip->tot_len) != recv_result)
            bug("packet's length is not equal to the amount of bytes received");

        struct icmphdr *icmp = (void *)(reply_buffer + sizeof(struct iphdr));

        if (icmp->type != ICMP_ECHOREPLY) bug("expected an ICMP echo reply");

        // "Internet Header Length is the length of the internet header in 32 bit words".
        // https://datatracker.ietf.org/doc/html/rfc791 "Internet protocol"
        size_t icmp_size = ntohs(ip->tot_len) - ip->ihl * 4;

        char *ip_string_static = inet_ntoa((struct in_addr){ .s_addr = ip->saddr });
        char *ip_string = try(0, "strdup failed", strdup(ip_string_static));

        wip(time)
        printf(
            "%zu bytes from %s: icmp_seq=%d ttl=%d time=[...] ms\n",
            icmp_size,
            ip_string,
            ntohs(icmp->un.echo.sequence),
            ip->ttl
        );

        free(ip_string);

        wip(dump the IPv4 header if -v is given if bad message comes)
        if (0)
            printf(
                "IP Hdr Dump:\n"
                "Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n" wip()
                "ICMP: type [...], code [...], size [...], id [...], seq [...]\n" wip()
            );

        return (struct ping_result) {
            .have_received_reply = true,
            wip(time)
        };
    }
}
