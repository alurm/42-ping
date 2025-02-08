# Test cases

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

# https://stackoverflow.com/questions/70175164/icmp-timestamps-added-to-ping-echo-requests-in-linux-how-are-they-represented-t.

"Including the UNIX timestamp of the time of transmission in the first data bytes of the ICMP Echo message is a trick/optimization the original ping by Mike Muuss used to avoid keeping track of it locally"
