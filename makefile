sources := $(addprefix c/, \
	$(addsuffix .c, \
		main \
		must \
		calculate_icmp_checksum \
		make_new_echo_request_packet \
		open_and_configure_raw_socket \
		lookup_ip \
		ping_once \
		ping_loop \
		print_ping_footer \
		print_ping_header \
		set_program_options \
	) \
)

dependencies = $(sources) makefile c/library.h
common_arguments = -W{all,extra} -lm --std c23 -o $@ $(sources)

ft_ping: $(dependencies)
	cc -D release -Werror $(common_arguments)

wip: $(dependencies)
	cc -g $(common_arguments)

.PHONY: valgrind
valgrind: wip; sudo valgrind ./wip google.com

# The school 42 boilerplate.
include 42.mk
