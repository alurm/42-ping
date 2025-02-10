#define _POSIX_C_SOURCE 200809L

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
    // 21 is an arbitrary value.
    echo_request_data_size = wip() 21,
    // Note: "All hosts must be prepared to accept datagrams of up to 576 octets".
    // https://www.rfc-editor.org/rfc/rfc791.html "Internet protocol".
    echo_request_icmp_packet_size = echo_request_data_size + sizeof(struct icmphdr),

    max_packet_size = 1 << 16,
};

struct program_options {
    bool have_time_to_live;
    uint8_t time_to_live;
    bool verbose;
    char *host;
    uint16_t identifier;
};

struct ping_result {
    bool have_received_reply;
    struct timeval time;
};

uint16_t calculate_icmp_checksum(struct icmphdr *, size_t);
void must(bool, char *);
struct icmphdr *make_new_echo_request_packet(uint16_t);
int open_and_configure_raw_socket(struct program_options);
struct sockaddr_in lookup_ip(char *);
struct ping_result ping_once(int, struct sockaddr_in, struct program_options);
[[noreturn]] void bug(char *);
struct program_options set_program_options(int, char **);
