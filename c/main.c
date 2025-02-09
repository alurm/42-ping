#include "lib.h"
#include "sys/types.h"

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h> // For strcmp.

struct raw_program_options {
    enum {
        raw_program_options_help_flag_set,
        raw_program_options_unknown_flag_set,
        raw_program_options_required_argument_is_not_provided_for,
        raw_program_options_no_hosts,
        raw_program_options_too_many_hosts,
        raw_program_options_are_normal,
    } type;

    union {
        char no_initializer;
        char *unknown_flag;
        char *required_argument_is_not_provided_for;
        struct normal_raw_program_options {
            bool be_verbose;

            bool time_to_live_specified;
            char *time_to_live;

            // We don't allow multiple hosts to be specified.
            // Having multiple hosts doesn't make sense given the flags we implement.
            char *host;
        } normal;
    };
};

[[noreturn]] void bug(char *error_message) {
    fprintf(stderr, "ping: bug: %s\n", error_message);
    exit(1);
}

// --key=value style of options is not supported.
// Use --key value instead.
bool parse_flag(struct raw_program_options *options, int argc, int *i, char *arg, char **argv) {
    // We know that arg[0] is '-'.
    switch (arg[1]) {
    case '?':
        *options = (struct raw_program_options){ raw_program_options_help_flag_set, .no_initializer = 0 };
        return false;
    case 'v':
        options->normal.be_verbose = true;
        return true;
    case '-':
        {
            char *long_flag = arg + 2;
            if (strcmp(long_flag, "ttl") == 0 || strcmp(long_flag, "time-to-live") == 0) {
                (*i)++;
                if (*i == argc) {
                    *options = (struct raw_program_options){
                        raw_program_options_required_argument_is_not_provided_for,
                        .required_argument_is_not_provided_for = arg,
                    };
                    return false;
                }
                options->normal.time_to_live_specified = true;
                options->normal.time_to_live = argv[*i];
                return true;
            } else if (strcmp(long_flag, "verbose") == 0) {
                options->normal.be_verbose = true;
                return true;
            } else if (strcmp(long_flag, "help") == 0) {
                *options = (struct raw_program_options){ raw_program_options_help_flag_set, .no_initializer = 0 };
                return false;
            } else {
                *options = (struct raw_program_options){
                    raw_program_options_unknown_flag_set,
                    .unknown_flag = arg,
                };
                return false;
            }
        }
    default:
        *options = (struct raw_program_options){
            raw_program_options_unknown_flag_set,
            .unknown_flag = arg,
        };
        return false;
    }
}

struct raw_program_options parse_argv(int argc, char **argv) {
    struct raw_program_options options = { raw_program_options_are_normal, .normal = { 0 } };

    for (int i = 0; i < argc; i++) {
        char *arg = argv[i];
        switch (arg[0]) {
        case '-':
            {
                bool normal = parse_flag(&options, argc, &i, arg, argv);
                if (!normal) return options;
                continue;
            }
        // Host.
        default:
            if (options.normal.host) return (struct raw_program_options){
                raw_program_options_too_many_hosts,
                .no_initializer = 0,
            };
            options.normal.host = arg;
        }
    }

    if (options.type != raw_program_options_are_normal)
        bug("options are abnormal");

    if (options.normal.host == 0) return (struct raw_program_options){
        .type = raw_program_options_no_hosts,
        .no_initializer = 0,
    };

    return options;
}

struct program_options {
    bool have_time_to_live;
    uint8_t time_to_live;

    bool verbose;

    char *host;
};

struct program_options parse_program_options(int argc, char **argv) {
    must(argc > 0, "expected the argv not to be empty");
    struct raw_program_options raw = parse_argv(argc - 1, argv + 1);
    switch (raw.type) {
    case raw_program_options_no_hosts:
        fprintf(
            stderr,
            "ping: missing host operand\n"
            "Try 'ping --help' for more information.\n"
        );
        exit(1);
    case raw_program_options_unknown_flag_set:
        fprintf(
            stderr,
            "ping: unrecognized option '%s'\n"
            "Try 'ping --help' for more information.\n",
            raw.unknown_flag
        );
        exit(1);
    case raw_program_options_help_flag_set:
        fprintf(
            stderr,
            "Usage: ping [OPTION...] HOST\n"
            "Send ICMP ECHO_REQUEST packets to a network host.\n"
            "\n"
            " Options:\n"
            "\n"
            "  --ttl N, --time-to-live N: specify N as time-to-live, must be between 1 and 255\n"
            "  -v, --verbose:             verbose output\n"
            "  -?, --help:                give this help list\n"
            "\n"
            "Report bugs to /dev/null."
        );
        exit(1);
    case raw_program_options_required_argument_is_not_provided_for:
        fprintf(
            stderr,
            "ping: option '%s' requires an argument\n"
            "Try 'ping --help' for more information\n",
            raw.required_argument_is_not_provided_for
        );
        exit(1);
    case raw_program_options_too_many_hosts:
        fprintf(
            stderr,
            "ping: too many hosts given\n"
            "Try 'ping --help' for more information\n"
        );
        exit(1);
    case raw_program_options_are_normal:
        {
            struct program_options options = { 0 };

            if (raw.normal.time_to_live_specified) {
                uint8_t time_to_live = atoi(raw.normal.time_to_live);
                // Maximal value to fit in a uint8_t is 255, taking 3 digits plus one for the null byte.
                char buffer[3 + 1];
                sprintf(buffer, "%d", time_to_live);
                if (strcmp(buffer, raw.normal.time_to_live) != 0 || time_to_live == 0) {
                    fprintf(
                        stderr,
                        "ping: time-to-live is either too big or too small: %s\n"
                        "Try 'ping --help' for more information\n",
                        raw.normal.time_to_live
                    );
                    exit(1);
                }
                options.have_time_to_live = true;
                options.time_to_live = time_to_live;
            }

            return options;
        }
    }

    bug("unreachable");
}

int main(int argc, char **argv) {
    struct program_options options = parse_program_options(argc, argv);
    (void)options wip();

    int raw_socket = open_and_configure_raw_socket();

    struct sockaddr_in ip = lookup_ip(); wip(bind?)

    wip() printf("PING [...] ([...]): [...] data bytes\n");

    wip(should be endless and have a sleep period)
    for (size_t i = 1; i <= 5; i++) ping_once(
        raw_socket,
        ip
    );

    wip() printf(
        "--- [...] ping statistics ---\n"
        "[...] packets transmitted, [...] packets received, [...]%% packet loss\n"
        "round-trip min/avg/max/stddev = [...]/[...]/[...]/[...] ms"
    );
}
