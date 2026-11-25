#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

/*
** Build an ICMP echo request in buf (PING_MAX_PACKET bytes, 8-byte aligned).
** Payload = send timestamp (struct timeval, only if it fits in -s bytes)
** followed by an incrementing pattern.
*/
size_t	ft_build_packet(t_ping *p, unsigned char *buf)
{
	struct icmphdr	*icmp = (struct icmphdr *)buf;
	struct timeval	now;
	unsigned char	*payload = buf + PING_ICMP_HDR_LEN;
	size_t			i;
	size_t			total = PING_ICMP_HDR_LEN + p->size;

	memset(buf, 0, total);
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = htons((unsigned short)p->id);
	icmp->un.echo.sequence = htons(p->seq);
	gettimeofday(&now, NULL);
	if ((size_t)p->size >= sizeof(now))
		memcpy(payload, &now, sizeof(now));
	for (i = sizeof(now); i < (size_t)p->size; i++)
		payload[i] = (unsigned char)i;
	icmp->checksum = 0;
	icmp->checksum = ft_checksum(buf, total);
	return (total);
}

/* Echo reply for us: print its line, with the RTT if it carries a timestamp. */
static void	ft_print_reply(t_ping *p, const struct iphdr *ip,
	const struct icmphdr *icmp, size_t icmp_len)
{
	struct timeval	sent;
	struct timeval	now;
	struct in_addr	src = {ip->saddr};
	double			rtt;

	printf("%zu bytes from %s: icmp_seq=%u ttl=%d", icmp_len, inet_ntoa(src),
		ntohs(icmp->un.echo.sequence), ip->ttl);
	p->received++;
	if (icmp_len >= PING_ICMP_HDR_LEN + sizeof(sent))
	{
		memcpy(&sent, (const unsigned char *)icmp + PING_ICMP_HDR_LEN,
			sizeof(sent));
		gettimeofday(&now, NULL);
		rtt = ft_timeval_diff_ms(&now, &sent);
		ft_stats_add_rtt(p, rtt);
		printf(" time=%.3f ms", rtt);
	}
	printf("\n");
}

/*
** Validate a received datagram (IP header + ICMP).
** Returns 1 if it was an echo reply for us (and prints its line), 0 otherwise.
*/
int	ft_parse_reply(t_ping *p, unsigned char *buf, ssize_t len)
{
	struct iphdr	*ip = (struct iphdr *)buf;
	struct icmphdr	*icmp;
	size_t			iphl;
	size_t			icmp_len;

	if (len < (ssize_t)sizeof(struct iphdr))
		return (0);
	iphl = ip->ihl * 4;
	if (iphl < sizeof(struct iphdr) || (size_t)len < iphl + PING_ICMP_HDR_LEN)
		return (0);
	icmp = (struct icmphdr *)(buf + iphl);
	icmp_len = len - iphl;
	if (ft_checksum(icmp, icmp_len) != 0)
		return (0);
	if (icmp->type != ICMP_ECHOREPLY)
		return (ft_handle_error(p, ip, icmp, icmp_len));
	if (ntohs(icmp->un.echo.id) != (unsigned short)p->id)
		return (0);
	ft_print_reply(p, ip, icmp, icmp_len);
	return (1);
}
