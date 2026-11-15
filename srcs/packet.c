#include <string.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

/* TODO: fill ICMP echo request (type 8, id, seq, timestamp, checksum). */
size_t	build_packet(t_ping *p, unsigned char *buf)
{
	(void)p;
	(void)buf;
	return (0);
}

/* TODO: validate IP + ICMP headers, match id, print line / ICMP error. */
int	parse_reply(t_ping *p, unsigned char *buf, ssize_t len)
{
	(void)p;
	(void)buf;
	(void)len;
	return (0);
}
