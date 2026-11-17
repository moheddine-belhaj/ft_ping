# ft_ping
42 Heilbronn Mastery project where I recreate the `ping` command, which allows you to test the accessibility of another machine through the IP network.

The subject is in [docs/en.subject_fr_ping.pdf](docs/en.subject_fr_ping.pdf) and the work plan is in [docs/PLAN.md](docs/PLAN.md).

## Status

Work in progress. The mandatory part is implemented but **not yet fully tested or compared** against the reference (inetutils-2.0).

### Done
- **Project skeleton**: Makefile (`all clean fclean re`, `-Wall -Wextra -Werror`, header dependency tracking, relinks only when needed), one source file per module.
- **Arguments**: `-v`, `-?`, one IPv4 address or hostname, and `--ttl N` (bonus, used to force errors).
- **Resolution**: IPv4 address/hostname/FQDN through `getaddrinfo`, with no reverse DNS on replies.
- **Raw ICMP socket**: TTL (`IP_TTL`) and receive timeout set.
- **Echo request**: type 8, id = pid, incrementing sequence, timestamp payload, 56 data bytes, Internet checksum.
- **Reply handling**: IP and ICMP header validation, checksum check, id matching, RTT computation and the per-packet line.
- **Main loop**: one packet per second, using `poll` between sends.
- **SIGINT**: stops the loop and prints the statistics (sent, received, loss, min/avg/max/stddev).
- **ICMP errors**: Time Exceeded, Destination Unreachable and similar are printed without stopping the program. With `-v` the original IP header is dumped as well.
- **Code style**: every function is prefixed `ft_` and is at most 25 lines (42 Norm).

### To do
- Run and verify everything with a raw socket (`sudo` or `cap_net_raw`).
- Compare the output format with inetutils-2.0 (spacing, error lines, `-v` dump).
- Robustness pass: valgrind, bad input, malformed packets.
- Bonus flags: `-n -w -W -s -p -f -l -r -T --ip-timestamp`.

## Build

```bash
make        # builds ./ft_ping
make re     # clean rebuild
```

## Usage

Raw sockets need privileges:

```bash
sudo ./ft_ping 127.0.0.1
# or, once:
sudo setcap cap_net_raw+ep ./ft_ping && ./ft_ping 127.0.0.1
```

```
./ft_ping [-v] [--ttl N] HOST
  -v         verbose output (dump the packet that triggered an ICMP error)
  --ttl N    set the time-to-live (1-255)
  -?         show help
```

Examples:

```bash
./ft_ping 8.8.8.8
./ft_ping --ttl 1 8.8.8.8      # forces "Time to live exceeded"
./ft_ping -v --ttl 1 8.8.8.8   # same, with the header dump
```

Press Ctrl-C to stop and print the statistics.

## Project layout

```
includes/ft_ping.h   shared structures and prototypes
srcs/main.c          entry point
srcs/args.c          option parsing
srcs/resolve.c       host resolution
srcs/socket.c        raw socket setup
srcs/packet.c        echo request builder, reply parser
srcs/icmp_error.c    ICMP error messages and -v header dump
srcs/checksum.c      Internet checksum
srcs/loop.c          send/receive loop
srcs/stats.c         statistics
srcs/utils.c         signals and time helpers
```

## Subject summary

- Written in **C**, with a **Makefile**. The executable is named `ft_ping`.
- Reference behaviour: the `ping` from **inetutils-2.0** (`ping -V`).
- Mandatory: `-v` and `-?`, a single IPv4 address or hostname, FQDN handling without reverse DNS on replies, and no crash on errors.
- Forbidden: calling the system `ping` or using the source of a standard ping.
- Bonus (only assessed if the mandatory part is perfect): extra flags such as `-f -l -n -w -W -p -r -s -T --ttl --ip-timestamp`.
- Output must match inetutils-2.0, except the RTT and reverse DNS. A ±30 ms tolerance is allowed.
