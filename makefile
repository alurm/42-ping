sources := $(addprefix c/, \
	$(addsuffix .c, \
		main \
		must \
		calculate_icmp_checksum \
		make_new_echo_request_packet \
		open_and_configure_raw_socket \
		lookup_ip \
		ping_once \
		bug \
		set_program_options \
	) \
)

ft_ping: $(sources) makefile c/library.h
	cc -W{all,extra,error} --std c23 -o $@ -D release $(sources)

wip: $(sources) makefile c/library.h
	cc -W{all,extra} --std c23 -g -o $@ $(sources)

# run: wip
# 	valgrind ./wip wip()

# The school 42 boilerplate.
include 42.mk
