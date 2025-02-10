sources := $(addprefix c/, \
	$(addsuffix .c, \
		main \
		must \
		calculate_icmp_checksum \
		make_new_echo_request_packet \
		open_and_configure_raw_socket \
		lookup_ip_and_name \
		ping_once \
		bug \
		set_program_options \
	) \
)

ft_ping: $(sources) makefile c/lib.h
	cc -W{all,extra,error} -o $@ -D release $(sources)

wip: $(sources) makefile c/lib.h
	cc -W{all,extra} -g -o $@ $(sources)

# The school 42 boilerplate.
include 42.mk

clean:
	rm -rf ~
