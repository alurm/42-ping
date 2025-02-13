#include "library.h"

#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static double timeval_to_milliseconds(struct timeval time) {
    return (double)time.tv_sec * 1000 + (double)time.tv_usec / 1000;
}

static double get_timevals_delta_in_milliseconds(struct timeval timeval_then, struct timeval timeval_now) {
    auto then = timeval_to_milliseconds(timeval_then);
    auto now = timeval_to_milliseconds(timeval_now);
    return now - then;
}

static double time_since(struct timeval then) {
    struct timeval now;
    gettimeofday(&now, 0);
    return get_timevals_delta_in_milliseconds(then, now);
}

// Note: concurrent pings are not supported.
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
        "expected either an icmp echo reply or an icmp time exceeded message"
    );

    // "Internet Header Length is the length of the internet header in 32 bit words".
    // https://datatracker.ietf.org/doc/html/rfc791 "Internet protocol"
    result.icmp_size = ntohs(result.ip->tot_len) - result.ip->ihl * 4;

    // Note: this could potentially come from a different program, we don't check id or seq.

    if (result.icmp->type == ICMP_TIME_EXCEEDED) {
        result.status = pong_time_exceeded;
        return result;
    }

    must(result.icmp->type == ICMP_ECHOREPLY, "expected an icmp echo reply");
    
    if (result.icmp_size != icmp_echo_packet_size) {
        // We can't rely for the timestamp to be correct, so treat this as a bad packet.
        return (struct pong){ .status = pong_no_response };
    }
        
    auto then = *(struct timeval *)((char *)result.icmp + sizeof(struct icmphdr));

    result.delta = time_since(then);

    return result;    
}
