# Description

This is a simple tool that behaves like the ping utility, checking availability of a given host. It does it's job by sending ICMP echo messages and receiving replies.

This is an implementation of a [project](https://cdn.intra.42.fr/pdf/pdf/149425/en.subject.pdf) at the school 42 [[web archive](https://web.archive.org/web/20250205123925/https://cdn.intra.42.fr/pdf/pdf/149425/en.subject.pdf)].

There are Nix packages for this project and inetutils 2.0 (which is used as a reference) under `packages/`.

A bunch of simple Bash tests can be run by typing `make test`.

# Usage

```text
sudo ./ft_ping [--verbose] [--help] [--count <n>] [--time-to-live <n>] <host>
```

# License

The license used for this project is MIT.
