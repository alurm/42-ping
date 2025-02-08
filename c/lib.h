#include <stdint.h>
#include <stddef.h>
#include <linux/icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef release
#define wip(...)
#endif

#define try(error_code, error_message, expression) ({ \
    typeof(expression) result = (expression); \
    if (result == error_code) { \
        perror("ping: " error_message); \
        exit(1); \
    } \
    result; \
})

uint16_t calculate_icmp_checksum(struct icmphdr *packet_as_icmp_header, size_t size);
void must(bool boolean, char *error_message);
struct icmphdr *make_new_echo_request_packet(void);
int open_and_configure_raw_socket(void);
struct sockaddr_in lookup_ip(void);
void ping_once(int raw_socket, struct sockaddr_in ip);

enum {
    // 21 is an arbitrary value.
    echo_request_data_size = wip() 21,
    // Note: "All hosts must be prepared to accept datagrams of up to 576 octets".
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol".
    echo_request_icmp_packet_size = echo_request_data_size + sizeof(struct icmphdr),
};
