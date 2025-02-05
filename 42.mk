# "Your Makefile [...] must contain the usual rules".

# The most useless line of code I have written in my life.
NAME = ft_ping

.PHONY: all re clean fclean

all: ft_ping

# https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html.
re:
	$(MAKE) fclean
	$(MAKE)

clean:

fclean: clean
	rm -f ft_ping
