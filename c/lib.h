#include <stdint.h>
#include <stddef.h>
#include <linux/icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>

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

enum {
    // 21 is an arbitrary value.
    echo_request_data_size = wip() 21,
    // Note: "All hosts must be prepared to accept datagrams of up to 576 octets".
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol".
    echo_request_icmp_packet_size = echo_request_data_size + sizeof(struct icmphdr),
};

struct program_options {
    bool have_time_to_live;
    uint8_t time_to_live;
    bool verbose;
    char *host;
    uint16_t identifier;
};

uint16_t calculate_icmp_checksum(struct icmphdr *, size_t);
void must(bool, char *);
struct icmphdr *make_new_echo_request_packet(uint16_t);
int open_and_configure_raw_socket(struct program_options);
struct sockaddr_in lookup_ip(char *);
void ping_once(int, struct sockaddr_in, struct program_options);
[[noreturn]] void bug(char *);
struct program_options set_program_options(int, char **);
