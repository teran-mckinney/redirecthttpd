# redirecthttpd

Redirects HTTP to HTTPS based on Host headers.

It's pretty ugly, but it works and it's fast.

May need adjustments to run on something other than FreeBSD.

Only listens on IPv6, so you need to set `sysctl net.inet6.ip6.v6only=0` if you want v4 as well.

### License

Released into the public domain (unlicense)
