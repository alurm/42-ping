#include "library.h"

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

// wip(mark as printf-like).
ssize_t signal_safe_printf(size_t buffer_size, char *format, ...) {
    // An array of variable length.
    char buffer[buffer_size];

    va_list args;
    va_start(args, format);
    size_t vsnprintfed = vsnprintf(buffer, sizeof buffer, format, args);
    va_end(args);
    size_t to_write = vsnprintfed;
    // Silent truncation.
    if (to_write >= buffer_size) to_write = buffer_size - 1;
    return write(1, buffer, to_write);
}

void print_ping_footer(void) {
    must(ping_statistics_data.transmitted != 0, "no packets transmitted");

    size_t packet_loss = (1 - ((double)ping_statistics_data.received / (double)ping_statistics_data.transmitted)) * 100;
    
    // Not handling an error.
    signal_safe_printf(
        2048, // Arbitrary.
        "--- %s ping statistics ---\n"
        "%zu packets transmitted, %zu packets received, %zu%% packet loss\n",
        ping_statistics_data.host,
        ping_statistics_data.transmitted,
        ping_statistics_data.received,
        packet_loss
    );

    if (ping_statistics_data.received > 0) {
        size_t n = ping_statistics_data.received;

        double *deltas = ping_statistics_data.deltas;

        double min = deltas[0];
        double max = deltas[0];
        double sum = 0;

        for (size_t i = 0; i < n; i++) {
            double item = deltas[i];
            if (item < min) min = item;
            if (item > max) max = item;
            sum += item;
        }

        double average = sum / n;

        double variance = 0;

        for (size_t i = 0; i < n; i++) {
            double item = deltas[i];
            double difference = average - item;
            variance += difference * difference;
        }

        double standard_deviation = sqrtl(variance);

        signal_safe_printf(
            2048, // Arbitrary.
            "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
            min,
            average,
            max,
            standard_deviation
        );
    }

    free(ping_statistics_data.deltas);
}
