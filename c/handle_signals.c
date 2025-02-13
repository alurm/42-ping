#include "library.h"

#include <signal.h>
#include <stdlib.h>

static void signal_handler(int) {
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
