#include "lib.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>

struct icmphdr;

// "The checksum is the 16-bit ones's complement of the one's complement sum of the ICMP message starting with the ICMP Type."
// https://en.wikipedia.org/wiki/Internet_checksum.
// https://datatracker.ietf.org/doc/html/rfc1071 "Computing the Internet checksum".
uint16_t calculate_icmp_checksum(struct icmphdr *packet_as_icmp_header, size_t size) {
    uint32_t sum = 0;

    char *packet = (void *)packet_as_icmp_header;

    for (size_t i = 0; i < size; i += 2) {
        // This one is more significant (larger) than the next octet, because the network octet order is big endian.
        // (https://datatracker.ietf.org/doc/html/rfc1700 "Assigned numbers".
        sum += packet[i] << 8;

        if (i + 1 < size) sum += packet[i + 1];

        // Looping since adding a carry may produce yet another carry.
        while (1) {
            uint16_t carry = sum >> 16;

            if (carry == 0) break;

            // Only keep the 16 least significant bits.
            uint32_t mask = ~(~0u << 16);

            uint16_t without_carry = sum & mask;

            sum = carry + without_carry;
        }
    }

    // Convert the sum to big endian and invert the bits ("one's complement").
    return ~htons(sum);
}
