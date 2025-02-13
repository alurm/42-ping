sources := $(wildcard c/*.c)

dependencies = $(sources) makefile 42.mk c/library.h
common_arguments = -O2 -W{all,extra} -lm --std c23 -o $@ $(sources)

ft_ping: $(dependencies)
	cc -D release -Werror $(common_arguments)

wip: $(dependencies)
	cc -g $(common_arguments)

.PHONY: test
test: wip
	./tests/run

# The school 42 boilerplate.
include 42.mk
