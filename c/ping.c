// https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/IWYUPragmas.md.
#include "netinet/in.h" // IWYU pragma: keep (for IPPROTO_ICMP).
#include "sys/socket.h"

/*
# raw(7), ip(7)

## CAP_NET_RAW

In order to create a raw socket, a process must have the CAP_NET_RAW capability in the user namespace that governs its network namespace.

## IP_HDRINCL

The IPv4 layer generates an IP header when sending a packet unless the IP_HDRINCL socket option is enabled on the socket.
For receiving, the IP header is always included in the packet.

## ICMP_FILTER

Enable a special filter for raw sockets bound to the IPPROTO_ICMP protocol.

## Error handling

Errors  originating  from the network are passed to the user only when the socket is connected or the IP_RECVERR flag is enabled.
Only EMSGSIZE and EPROTO are passed for compatibility.  With IP_RECVERR, all network errors are saved in the error queue.

## Bind(2), interfaces

A raw socket can be bound to a specific local address using the bind(2) call.
If it isn't bound, all packets with the specified IP protocol are received.  In addition, a raw socket can be bound to a specific network device using SO_BINDTODEVICE; see socket(7).

## Recvmsg, sendmsg

[Intentionally left empty]

## IP_TTL

Set or retrieve the current time-to-live field that is used in every packet sent from this socket.

## IP_RECVERR with IPPROTO_RAW

"If you want to receive all ICMP packets for a datagram socket, it is often better to use IP_RECVERR on that particular socket; see ip(7)."

## For receiving, the IP header is always included in the packet.

# How to test this?

ping kek
ping google.com
ping localhost
ping 1.2.3.4
ping
^C
^\

# Ping implementations

https://manpages.debian.org/unstable/inetutils-ping/ping.1.en.html

-v

"ICMP packets other than ECHO_RESPONSE that are received are listed"

"An IP header without options is 20 bytes"

Wireshark shows that pings current time in their payload, which is of size 40.

https://stackoverflow.com/questions/70175164/icmp-timestamps-added-to-ping-echo-requests-in-linux-how-are-they-represented-t.
*/

int open_socket(void) {
    socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
}
