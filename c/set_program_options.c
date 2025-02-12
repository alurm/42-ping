#include "library.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

            bool have_count;
            char *count;
        } normal;
    };
};

struct decimal_number_parse {
    bool ok;
    ssize_t number;
};

struct decimal_number_parse parse_decimal_number(char *string) {
    ssize_t number = atol(string);
    char *pointer;
    try(
        -1,
        "malloc failed",
        asprintf(&pointer, "%ld", number)
    );
    if (strcmp(pointer, string) != 0) {
        free(pointer);
        return (struct decimal_number_parse){ .ok = false };
    }
    free(pointer);
    return (struct decimal_number_parse){
        .ok = true,
        .number = number,
    };
}

bool match(char *string, char **options) {
    while (*options != 0) {
        if (strcmp(string, *options) == 0)
            return true;
        options++;
    }

    return false;
}

#define match(string, ...) match( \
    string, \
    (char*[]){ \
        __VA_ARGS__, \
        0 \
    } \
)

// Specifying multiple short options (-ab) in one arg is not supported.
// Use multiple args instead (-a -b).
// --key=value style of options is not supported.
// Use --key value instead.
bool parse_flag(struct raw_program_options *options, int argc, int *i, char *arg, char **argv) {
    // We know that arg[0] is '-'.
    char *flag = arg + 1;

    if (match(flag, "?", "-help")) {
        *options = (struct raw_program_options){ raw_program_options_help_flag_set, .no_initializer = 0 };
        return false;
    }
    else if (match(flag, "v", "-verbose")) {
        options->normal.be_verbose = true;
        return true;
    }
    // These take an argument.
    else if (match(flag, "-ttl", "-time-to-live", "c", "-count")) {
        (*i)++;
        if (*i == argc) {
            *options = (struct raw_program_options){
                raw_program_options_required_argument_is_not_provided_for,
                .required_argument_is_not_provided_for = arg,
            };
            return false;
        }

        if (0) {}
        else if (match(flag, "-ttl", "-time-to-live")) {
            options->normal.time_to_live_specified = true;
            options->normal.time_to_live = argv[*i];
        }
        else if (match(flag, "c", "-count")) {
            options->normal.have_count = true;
            options->normal.count = argv[*i];
        }
        return true;
    }
    else {
        *options = (struct raw_program_options){
            raw_program_options_unknown_flag_set,
            .unknown_flag = arg,
        };
        return false;
    }
}

struct raw_program_options parse_argv(int argc, char **argv) {
    struct raw_program_options options = { raw_program_options_are_normal, .normal = {} };

    for (int i = 0; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            bool normal = parse_flag(&options, argc, &i, arg, argv);
            if (!normal) return options;
            continue;
        } else {
            if (options.normal.host) return (struct raw_program_options){
                raw_program_options_too_many_hosts,
                .no_initializer = 0,
            };
            options.normal.host = arg;
        }
    }

    must(options.type == raw_program_options_are_normal, "options are abnormal");

    if (options.normal.host == 0) return (struct raw_program_options){
        .type = raw_program_options_no_hosts,
    };

    return options;
}

struct program_options set_program_options(int argc, char **argv) {
    must(argc > 0, "expected the argv not to be empty");
    auto raw = parse_argv(argc - 1, argv + 1);
    switch (raw.type)
    case raw_program_options_no_hosts: if (1) {
        fprintf(
            stderr,
            "ping: missing host operand\n"
            "Try 'ping --help' for more information.\n"
        );
        exit(1);
    } else
    case raw_program_options_unknown_flag_set: if (1) {
        fprintf(
            stderr,
            "ping: unrecognized option '%s'\n"
            "Try 'ping --help' for more information.\n",
            raw.unknown_flag
        );
        exit(1);
    } else
    case raw_program_options_help_flag_set: if (1) {
        fprintf(
            stderr,
            "Usage: ping [OPTION...] HOST\n"
            "Send ICMP ECHO_REQUEST packets to a network host.\n"
            "\n"
            " Options:\n"
            "\n"
            "  -?, --help:                give this help list\n"
            "  -v, --verbose:             verbose output\n"
            "  --ttl N, --time-to-live N: specify N as time-to-live, must be between 1 and 255\n"
            "  -c N, --count N:           stop after sending N packets\n"
            "\n"
            "Report bugs to /dev/null."
        );
        exit(1);
    } else
    case raw_program_options_required_argument_is_not_provided_for: if (1) {
        fprintf(
            stderr,
            "ping: option '%s' requires an argument\n"
            "Try 'ping --help' for more information\n",
            raw.required_argument_is_not_provided_for
        );
        exit(1);
    } else
    case raw_program_options_too_many_hosts: if (1) {
        fprintf(
            stderr,
            "ping: too many hosts given\n"
            "Try 'ping --help' for more information\n"
        );
        exit(1);
    } else
    case raw_program_options_are_normal: if (1) {
        ping_statistics_data.host = raw.normal.host;
        ping_statistics_data.verbose = raw.normal.be_verbose;
        // The identifier occupies 16 bits.
        ping_statistics_data.identifier = htons(getpid() % (1 << 16));

        struct program_options options = {};

        if (raw.normal.time_to_live_specified) {
            auto p = parse_decimal_number(raw.normal.time_to_live);
            must(
                p.ok && p.number > 0 && p.number < 256,
                "invalid time-to-live\n"
                "Try 'ping --help' for more information"
            );

            options.have_time_to_live = true;
            options.time_to_live = p.number;
        }

        if (raw.normal.have_count) {
            auto p = parse_decimal_number(raw.normal.count);
            must(
                p.ok && p.number >= 0,
                "invalid count\n"
                "Try 'ping --help' for more infromation"
            );

            options.have_count = true;
            options.count = p.number;
        }

        return options;
    }
}
