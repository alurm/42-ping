#include "library.h"

#include <unistd.h>

void ping_loop(
    int socket,
    struct sockaddr_in ip,
    struct program_options options
) {
    for (;;) {
        if (options.have_count && ping_statistics_data.transmitted == options.count) break;

        // Copying how inetutils' ping works.
        ping_statistics_data.transmitted++;

        auto ping = ping_once(socket, ip, ping_statistics_data.verbose);

        if (ping.have_received_reply)
            push(ping_statistics_data.deltas, ping_statistics_data.received, ping.delta);

        sleep(ping_interval_seconds);
    }
}
