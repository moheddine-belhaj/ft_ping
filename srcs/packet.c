#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

/*
** Build an ICMP echo request in buf (at least PING_PACKET_LEN bytes, 8-byte aligned).
** Payload = send timestamp (struct timeval) followed by an incrementing pattern.
*/
size_t	ft_build_packet(t_ping *p, unsigned char *buf)
{
	struct icmphdr	*icmp = (struct icmphdr *)buf;
	struct timeval	now;
	unsigned char	*payload = buf + PING_ICMP_HDR_LEN;
	size_t			i;

	memset(buf, 0, PING_PACKET_LEN);
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = htons((unsigned short)p->id);
	icmp->un.echo.sequence = htons(p->seq);
	gettimeofday(&now, NULL);
	memcpy(payload, &now, sizeof(now));
	for (i = sizeof(now); i < PING_DATA_LEN; i++)
		payload[i] = (unsigned char)i;
	icmp->checksum = 0;
	icmp->checksum = ft_checksum(buf, PING_PACKET_LEN);
	return (PING_PACKET_LEN);
}

/*
** Validate a received datagram (IP header + ICMP).
** Returns 1 if it was an echo reply for us (and prints its line), 0 otherwise.
*/
int	ft_parse_reply(t_ping *p, unsigned char *buf, ssize_t len)
{
	struct iphdr	*ip = (struct iphdr *)buf;
	struct icmphdr	*icmp;
	struct timeval	sent;
	struct timeval	now;
	size_t			iphl;
	size_t			icmp_len;
	double			rtt;

	if (len < (ssize_t)sizeof(struct iphdr))
		return (0);
	iphl = ip->ihl * 4;
	if (iphl < sizeof(struct iphdr) || (size_t)len < iphl + PING_ICMP_HDR_LEN)
		return (0);
	icmp = (struct icmphdr *)(buf + iphl);
	icmp_len = len - iphl;
	if (ft_checksum(icmp, icmp_len) != 0)
		return (0);
	if (icmp->type != ICMP_ECHOREPLY
		|| ntohs(icmp->un.echo.id) != (unsigned short)p->id)
		return (0);
	if (icmp_len < PING_ICMP_HDR_LEN + sizeof(sent))
		return (0);
	memcpy(&sent, (unsigned char *)icmp + PING_ICMP_HDR_LEN, sizeof(sent));
	gettimeofday(&now, NULL);
	rtt = ft_timeval_diff_ms(&now, &sent);
	ft_stats_add_rtt(p, rtt);
	printf("%zu bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
		icmp_len, p->ip_str, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
	return (1);
}
