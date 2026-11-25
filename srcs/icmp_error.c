#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

static const char	*ft_icmp_error_str(int type, int code)
{
	static const char	*unreach[] = {"Destination Net Unreachable",
		"Destination Host Unreachable", "Destination Protocol Unreachable",
		"Destination Port Unreachable", "Fragmentation needed and DF set",
		"Source Route Failed"};

	if (type == ICMP_DEST_UNREACH)
		return (code >= 0 && code < 6 ? unreach[code] : "Dest Unreachable, Bad Code");
	if (type == ICMP_TIME_EXCEEDED)
		return (code == 0 ? "Time to live exceeded" : "Frag reassembly time exceeded");
	if (type == ICMP_SOURCE_QUENCH)
		return ("Source Quench");
	if (type == ICMP_REDIRECT)
		return ("Redirect");
	if (type == ICMP_PARAMETERPROB)
		return ("Parameter problem");
	return (NULL);
}

/* -v: dump the IP header of the packet that triggered the error. */
static void	ft_dump_inner(const struct iphdr *in, const struct icmphdr *inner,
	size_t ihl)
{
	const unsigned char	*raw = (const unsigned char *)in;
	struct in_addr		src = {in->saddr};
	struct in_addr		dst = {in->daddr};
	size_t				i;

	printf("IP Hdr Dump:\n ");
	for (i = 0; i < sizeof(*in); i++)
		printf("%02x%s", raw[i], (i % 2) ? " " : "");
	printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
	printf(" %1x  %1x  %02x %04x %04x", in->version, in->ihl, in->tos,
		ntohs(in->tot_len), ntohs(in->id));
	printf("   %1x %04x", (ntohs(in->frag_off) & 0xe000) >> 13,
		ntohs(in->frag_off) & 0x1fff);
	printf("  %02x  %02x %04x", in->ttl, in->protocol, ntohs(in->check));
	printf(" %s ", inet_ntoa(src));
	printf(" %s \n", inet_ntoa(dst));
	printf("ICMP: type %u, code %u, size %zu", inner->type, inner->code,
		(size_t)ntohs(in->tot_len) - ihl);
	if (inner->type == ICMP_ECHO || inner->type == ICMP_ECHOREPLY)
		printf(", id 0x%04x, seq 0x%04x", ntohs(inner->un.echo.id),
			ntohs(inner->un.echo.sequence));
	printf("\n");
}

/*
** ICMP error (dest unreachable, time exceeded...) whose payload is our echo request.
** Prints it without stopping the program; returns 1 if it concerned us.
*/
int	ft_handle_error(t_ping *p, const struct iphdr *ip,
	const struct icmphdr *icmp, size_t icmp_len)
{
	const struct iphdr		*in;
	const struct icmphdr	*inner;
	const char				*msg = ft_icmp_error_str(icmp->type, icmp->code);
	struct in_addr			src = {ip->saddr};
	size_t					in_ihl;

	if (!msg || icmp_len < PING_ICMP_HDR_LEN + sizeof(struct iphdr))
		return (0);
	in = (const struct iphdr *)((const unsigned char *)icmp + PING_ICMP_HDR_LEN);
	in_ihl = in->ihl * 4;
	if (in_ihl < sizeof(struct iphdr)
		|| icmp_len < PING_ICMP_HDR_LEN + in_ihl + PING_ICMP_HDR_LEN)
		return (0);
	inner = (const struct icmphdr *)((const unsigned char *)in + in_ihl);
	if (in->protocol != IPPROTO_ICMP || inner->type != ICMP_ECHO
		|| ntohs(inner->un.echo.id) != (unsigned short)p->id)
		return (0);
	printf("%zu bytes from %s: %s\n", icmp_len, inet_ntoa(src), msg);
	if (p->verbose)
		ft_dump_inner(in, inner, in_ihl);
	return (1);
}
