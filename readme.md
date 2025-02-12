# Description

This is a simple tool that behaves like the ping utility, checking availability of a given host. It does it's job by sending ICMP echo messages and receiving replies.

This is an implementation of a [project](https://cdn.intra.42.fr/pdf/pdf/149425/en.subject.pdf) at the school 42 [[web archive](https://web.archive.org/web/20250205123925/https://cdn.intra.42.fr/pdf/pdf/149425/en.subject.pdf)].

# Test cases (wip())

```
ping kek
ping google.com
ping localhost
ping 1.2.3.4
^\
ping
^C
^\
sudo nix shell .#inetutils -c ping google.com -v --ttl 2
```

# License

The license used for this project is MIT.
