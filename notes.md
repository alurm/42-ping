# Manuals

raw(7), ip(7)

# `CAP_NET_RAW`

"In order to create a raw socket, a process must have the `CAP_NET_RAW` capability in the user namespace that governs its network namespace."

# `IP_HDRINCL`

"The IPv4 layer generates an IP header when sending a packet unless the `IP_HDRINCL` socket option is enabled on the socket. For receiving, the IP header is always included in the packet.

For receiving, the IP header is always included in the packet."

(The current assumption is that this option is not needed for this project.)

# `ICMP_FILTER`

"Enable a special filter for raw sockets bound to the `IPPROTO_ICMP` protocol.

The value has a bit set for each ICMP message type which should be filtered out.  The default is to filter no ICMP messages."

(The current assumption is that the --verbose flag may need to print all types of ICMP messages received. This is challenged since inetutils' ping doesn't seem to be actually doing that.)

(It seems like implementing IP header dumps for -v would be enough. Then it seems like `ICMP_FILTER` can be used.)

# `IP_RECVERR`

(Not sure if this is necessary. See `/IP_RECVERR` in ip(7) and raw(7) for more.)

# bind(2), interfaces

"A raw socket can be bound to a specific local address using the bind(2) call. If it isn't bound, all packets with the specified IP protocol are received.  In addition, a raw socket can be bound to a specific network device using `SO_BINDTODEVICE`; see socket(7)."

(Not sure if this is necessary.)

# recvmsg(2), sendmsg(2), flags

[Intentionally left empty.]

# `IP_TTL`

Set or retrieve the current time-to-live field that is used in every packet sent from this socket.

(This is used.)

# Test cases

```
ping kek
ping google.com
ping localhost
ping 1.2.3.4
^\
ping
^C
^\
sudo nix shell .#inetutils -c ping google.com -v --ttl 2
```

# https://manpages.debian.org/unstable/inetutils-ping/ping.1.en.html

"ICMP packets other than `ECHO_RESPONSE` that are received are listed"

"An IP header without options is 20 bytes"

# https://stackoverflow.com/questions/70175164/icmp-timestamps-added-to-ping-echo-requests-in-linux-how-are-they-represented-t.

"Including the UNIX timestamp of the time of transmission in the first data bytes of the ICMP Echo message is a trick/optimization the original ping by Mike Muuss used to avoid keeping track of it locally"
