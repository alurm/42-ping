ft_ping: c/main.c makefile
	cc -W{all,extra,error} -o $@ $< -D release

wip: c/main.c makefile
	cc -W{all,extra} -o $@ $<

# The school 42 boilerplate.
include 42.mk
