# ft_ping — Project Plan

Reference implementation: **inetutils-2.0** (`ping -V`). Language: **C**, with a Makefile. Using the system `ping` or its sources is forbidden.

## 0. Setup
- [ ] Debian VM (>= 7.0, kernel > 3.14) with `gcc`, `make`, `gdb`, `valgrind`, `tcpdump`
- [ ] inetutils-2.0 `ping` available as a behaviour reference (never copy its source)
- [ ] Raw sockets need root or `sudo setcap cap_net_raw+ep ./ft_ping`

## 1. Skeleton
- [x] `Makefile`: `all clean fclean re`, `-Wall -Wextra -Werror`, header deps (`-MMD`), relinks only when needed
- [x] `includes/ft_ping.h`: `t_ping` state struct and prototypes
- [x] `srcs/`: `main`, `args`, `resolve`, `socket`, `packet`, `checksum`, `loop`, `stats`, `utils`
- [x] `.gitignore`

## 2. Mandatory part
1. **Arguments** (`args.c`)
   - [x] `-v`, `-?`, single host operand
   - [ ] Match inetutils error messages and exit codes
2. **Resolution** (`resolve.c`)
   - [x] IPv4 address or hostname/FQDN via `getaddrinfo`
   - [ ] No reverse DNS on replies; header shows host and IP like inetutils
3. **Socket** (`socket.c`)
   - [x] `SOCK_RAW` / `IPPROTO_ICMP`, `IP_TTL`, `SO_RCVTIMEO`
4. **Packet** (`packet.c`, `checksum.c`)
   - [x] Internet checksum
   - [x] Build ICMP echo request (type 8, id = pid, seq, timestamp payload, 56 data bytes)
   - [x] Parse reply: IP header, ICMP header, match id
5. **Loop** (`loop.c`)
   - [x] One packet per second: `sendto` then `recvfrom` or `recvmsg`
   - [x] RTT with `gettimeofday` or `clock_gettime`
   - [x] Line format: `64 bytes from <ip>: icmp_seq=N ttl=T time=X.XXX ms`
6. **SIGINT and statistics** (`stats.c`, `utils.c`)
   - [x] Signal flag and summary printer
   - [ ] Verify the summary format against the reference
7. **ICMP errors**
   - [x] Time exceeded, destination unreachable: print them and **do not quit**
   - [x] With `-v`, dump the offending packet header
8. **Robustness**
   - [ ] No segfault, bus error or double free
   - [ ] `valgrind` clean
   - [ ] Bad host, unreachable network, no permission, malformed reply

## 3. Verification
- Compare side by side with the reference on `127.0.0.1`, `8.8.8.8`, a hostname, an invalid host, and a forced low TTL
- The output must match inetutils-2.0 except the RTT value and reverse DNS
- Inspect packets with `tcpdump -i any icmp` (checksum, id, seq, TTL)
- Timing tolerance: ±30 ms on reception
- Test Ctrl-C at different moments
- Run `valgrind --leak-check=full`

## 4. Bonus (only if the mandatory part is perfect)
Each distinct feature counts once; `-V`, `--usage` and `--echo` do not count.
- [ ] `-n`, `-w`, `-W`, `-s`, `--ttl`
- [ ] `-p`, `-f`, `-l`, `-r`, `-T`, `--ip-timestamp`

## 5. Housekeeping
- Small commits on `dev`; merge to `main` at stable milestones
- Keep the README updated with build, usage and testing notes

## Suggested order of work
1. `ft_build_packet` + a single send/receive (`ping -c 1` equivalent)
2. The 1 s loop + RTT + per-packet output
3. SIGINT + statistics
4. ICMP error handling + `-v`
5. Output diffing against inetutils, robustness pass
6. Bonus flags
