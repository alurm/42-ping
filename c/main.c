#include "library.h"

struct ping_statistics_data ping_statistics_data = {};

int main(int argc, char **argv) {
    auto options = set_program_options(argc, argv);
    auto socket = open_and_configure_raw_socket(options);
    auto ip_and_string = lookup_ip(ping_statistics_data.host);
    print_ping_header(ip_and_string.string);
    ping_loop(socket, ip_and_string.ip, options);
    print_ping_footer();
}
