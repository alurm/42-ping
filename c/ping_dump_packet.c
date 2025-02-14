#include "library.h"

#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <stdio.h>

void ping_dump_packet(struct iphdr *ip, struct icmphdr *icmp, size_t icmp_size) {
    printf("IP Hdr Dump:\n");

    size_t twobytes = ip->ihl * 2;
    auto packet = (char *)ip;

    printf(" ");
    for (size_t i = 0; i < twobytes; i++)
        printf("%.2x%.2x ", packet[i * 2], packet[i * 2 + 1]);
    printf("\n");

    // An example header dump:
    //
    // IP Hdr Dump:
    //  4500 0054 04ce 4000 0101 49a0 0a13 d6bf 8efa bb6e 
    // Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
    //  4  5  00 0054 ca99   2 0000  01  01 21f4 10.19.214.191  216.58.212.14 
    // ICMP: type 8, code 0, size 64, id 0x269e, seq 0x0000

    char *source = try(0, "malloc failed", strdup(inet_ntoa((struct in_addr){ .s_addr = ip->saddr })));
    char *destination = try(0, "malloc failed", strdup(inet_ntoa((struct in_addr){ .s_addr = ip->daddr })));

    // Format used as a reference: inetutils-2.0's ping/ping_echo:/^print_ip_header/.
    // (Fun fact: this is probably a GPL violation, since the code below is licensed under MIT.)

    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
    printf(
        " %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %s %s ",
        ip->version,
        ip->ihl,
        ip->tos,
        ntohs(ip->tot_len),
        ntohs(ip->id),
        (ntohs(ip->frag_off) & ~((1 << 13) - 1)) >> 13,
        ntohs(ip->frag_off) & ((1 << 13) - 1),
        ip->ttl,
        ip->protocol,
        ntohs(ip->check),
        source,
        destination
    );

    for (size_t i = 0; i < (ip->ihl * 4) - sizeof *ip; i++) printf("%02x", (packet + sizeof(*ip))[i]);

    printf("\n");
    
    free(source);
    free(destination);

    printf(
        "ICMP: type %u, code %u, size %zu",
        icmp->type,
        icmp->code,
        icmp_size
    );

    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY)
        printf(
            ", id 0x%04x, seq 0x%04x",
            icmp->un.echo.id,
            icmp->un.echo.sequence
        );
    
    printf("\n");
}
