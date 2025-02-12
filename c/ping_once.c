#include "bits/types/struct_timeval.h"
#include "library.h"
#include "sys/time.h"

#include <linux/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

void send_ping(int socket, struct sockaddr_in ip) {
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

double timeval_to_milliseconds(struct timeval time) {
    return (double)time.tv_sec * 1000 + (double)time.tv_usec / 1000;
}

double get_timevals_delta_in_milliseconds(struct timeval timeval_then, struct timeval timeval_now) {
    auto then = timeval_to_milliseconds(timeval_then);
    auto now = timeval_to_milliseconds(timeval_now);
    return now - then;
}

double time_since(struct timeval then) {
    struct timeval now;
    gettimeofday(&now, 0);
    return get_timevals_delta_in_milliseconds(then, now);
}

struct pong {
    enum {
        pong_time_exceeded,
        pong_corrupt,
        pong_ok,
        pong_no_response,
    } status;
    struct iphdr *ip;
    struct icmphdr *icmp;
    size_t icmp_size;
    double delta;
};

struct pong receive_pong(int socket, char *buffer) {
    struct pong result = { .status = pong_ok };

    ssize_t recv_result = recv(socket, buffer, max_packet_size, 0);

    if (recv_result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return (struct pong){ .status = pong_no_response };
        else {
            perror("ping: failed to receive a packet");
            exit(1);
        }
    }

    result.ip = (struct iphdr *)buffer;
    must(ntohs(result.ip->tot_len) == recv_result, "response packet's length is not equal to the amount of bytes received");

    result.icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));
    must(
        result.icmp->type == ICMP_ECHOREPLY || result.icmp->type == ICMP_TIME_EXCEEDED,
        "ICMP: expected either an echo reply or a timeout message"
    );

    // "Internet Header Length is the length of the internet header in 32 bit words".
    // https://datatracker.ietf.org/doc/html/rfc791 "Internet protocol"
    result.icmp_size = ntohs(result.ip->tot_len) - result.ip->ihl * 4;

    if (result.icmp->type == ICMP_TIME_EXCEEDED) {
        result.status = pong_time_exceeded;
        return result;
    }

    wip(should corrupt packets be marked as received? then we cannot use received as length for deltas?)
    wip(consider adding the corruption reason)
    if (
        result.icmp->type != ICMP_ECHOREPLY ||
        result.icmp_size != icmp_echo_packet_size ||
        result.icmp->un.echo.id != ping_statistics_data.identifier ||
        ntohs(result.icmp->un.echo.sequence) != ping_statistics_data.current_sequence - 1 ||
        0
    ) {
        result.status = pong_corrupt;
        return result;
    }

    auto then = *(struct timeval *)((char *)result.icmp + sizeof(struct icmphdr));

    result.delta = time_since(then);

    return result;
}

void ping_dump_packet(struct iphdr *ip, struct icmphdr *icmp, size_t icmp_size) { wip()
    (void)ip; (void)icmp; (void)icmp_size;
    // printf(
    //     "IP Hdr Dump:\n"
    //     "Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n" wip()
    //     "ICMP: type [...], code [...], size [...], id [...], seq [...]\n" wip()
    // );

    // ip->version;
    // ip->ihl;
    // ip->tos;
    // ip->tot_len;
    // ip->id;
    // // flg?
    // ip->frag_off;
    // ip->ttl;
    // ip->protocol;
    // ip->check;
    // ip->saddr;
    // ip->daddr;
    
    // icmp->type;
    // icmp->code;
    // icmp_size;
    // // id?
    // // seq?
}

struct ping_result ping_once(int socket, struct sockaddr_in ip, bool verbose) {
    wip(is the pinging error logic correct?)
    send_ping(socket, ip);

    char buffer[max_packet_size];
    auto pong = receive_pong(socket, buffer);

    switch (pong.status)
    case pong_no_response: if (1) {
        return (struct ping_result){ .have_received_reply = false };
    } else
    case pong_ok:
    case pong_corrupt:
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
            if (pong.status == pong_time_exceeded)
                printf("Time exceeded\n"); wip(message should be correct)
            else
                printf("the packet is corrupt\n"); wip(this is weird)
            if (verbose) ping_dump_packet(pong.ip, pong.icmp, pong.icmp_size);
            wip(what to return if time exceeded?)
            return (struct ping_result){ .have_received_reply = false };
        }
    }
}
