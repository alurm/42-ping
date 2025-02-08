#include <linux/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include "lib.h"

void ping_once(int raw_socket, struct sockaddr_in ip) {
    {
        struct icmphdr *request = make_new_echo_request_packet();

        wip(should errors be handled more gracefully?)
        try(
            -1,
            "failed to send an echo request",
            sendto(raw_socket, request, echo_request_icmp_packet_size, 0, (void *)&ip, sizeof ip)
        );
        
        free(request);
    }

    {
        // The length of a datagram is up to 65535 octets.
        // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol"
        char reply_buffer[1 << 16];

        wip(should errors be handled more gracefully?)
        try(
            -1,
            "failed to receive a packet",
            wip(recv) recv(raw_socket, reply_buffer, sizeof reply_buffer, 0)
        );

        struct iphdr *ip = (void *)reply_buffer;
        struct icmphdr *icmp = (void *)(reply_buffer + sizeof(struct iphdr));

        wip() must(icmp->type == ICMP_ECHOREPLY, "expected an ICMP echo reply");

        // "Internet Header Length is the length of the internet header in 32 bit words".
        // https://datatracker.ietf.org/doc/html/rfc791 "Internet protocol"
        size_t icmp_size = ntohs(ip->tot_len) - ip->ihl * 4;

        wip() printf(
            "%zu bytes from [...]: icmp_seq=%d ttl=%d time=[...] ms\n",
            icmp_size,
            ntohs(icmp->un.echo.sequence),
            ip->ttl
        );

        wip(dump the IPv4 header if -v is given)
    }
}
