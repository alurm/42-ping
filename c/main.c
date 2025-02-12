#include "library.h"

#include <signal.h>
#include <unistd.h>

struct ping_statistics_data ping_statistics_data = {};

void signal_handler(int) {
    print_ping_footer();
    exit(0);
}

void handle_signals(void) {
    sigaction(
        SIGINT,
        &(struct sigaction){
            .sa_flags = 0,
            .sa_handler = signal_handler,
        },
        0
    );
}

int main(int argc, char **argv) {
    handle_signals();
    auto options = set_program_options(argc, argv);
    auto socket = open_and_configure_raw_socket(options);
    auto ip_and_string = lookup_ip(ping_statistics_data.host);
    print_ping_header(ip_and_string.string);
    ping_loop(socket, ip_and_string.ip, options);
    print_ping_footer();
}
