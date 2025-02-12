#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <stddef.h>
#include <linux/icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/ip.h>

#ifndef release
#define wip(...)
#endif

// Count leading zeros.
// https://jameshfisher.com/2018/03/30/round-up-power-2.
// https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html.
#define nearest_power_of_two_above(number) (1 << ((sizeof(number) * 8) - __builtin_clzl(number)))

#define try(error_code, error_message, expression) ({ \
    auto try_macro_result = (expression); \
    if (try_macro_result == error_code) { \
        perror("ping: " error_message); \
        exit(1); \
    } \
    try_macro_result; \
})

#define push(array_place_expression, size_place_expression, object) do { \
    auto push_macro_array_pointer = &(array_place_expression); \
    auto push_macro_size = (size_place_expression)++; \
    *push_macro_array_pointer = try( \
        0, \
        "realloc failed", \
        realloc(*push_macro_array_pointer, (push_macro_size + 1) * sizeof(object)) \
    ); \
    (*push_macro_array_pointer)[push_macro_size] = object; \
} while (0)

enum {
    // 56 is the amount inetutils sends on my machine.
    // The subject suggests we have to do the same.
    echo_request_data_size = 56,
    // Note: "All hosts must be prepared to accept datagrams of up to 576 octets".
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol".
    icmp_echo_packet_size = echo_request_data_size + sizeof(struct icmphdr),

    // The length of a datagram is up to 65535 octets.
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol"
    max_packet_size = 1 << 16,

    receive_timeout_seconds = 3,

    ping_interval_seconds = 1,
};

static_assert(echo_request_data_size >= sizeof(struct timeval));

struct program_options {
    bool have_time_to_live;
    uint8_t time_to_live;
    bool have_count;
    size_t count;
};

struct ping_result {
    bool have_received_reply;
    double delta;
};

struct ip_lookup_result {
    struct sockaddr_in ip;
    char *string;
};

extern struct ping_statistics_data {
    double *deltas;
    size_t transmitted;
    size_t received;
    char *host;
    bool verbose;
    size_t current_sequence;
    size_t identifier;
} ping_statistics_data;

uint16_t calculate_icmp_checksum(struct icmphdr *, size_t);
void must(bool, char *);
struct icmphdr *make_new_echo_request_packet(void);
int open_and_configure_raw_socket(struct program_options);
struct ip_lookup_result lookup_ip(char *);
struct ping_result ping_once(int, struct sockaddr_in, bool);
struct program_options set_program_options(int, char **);
void print_ping_header(char *);
void print_ping_footer(void);
void ping_loop(int, struct sockaddr_in, struct program_options);
