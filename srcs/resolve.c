#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ft_ping.h"

int	ft_resolve_target(t_ping *p)
{
	struct addrinfo	hints;
	struct addrinfo	*res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	if (getaddrinfo(p->target, NULL, &hints, &res) != 0)
	{
		fprintf(stderr, "%s: unknown host\n", p->prog);
		return (-1);
	}
	memcpy(&p->dest, res->ai_addr, sizeof(p->dest));
	freeaddrinfo(res);
	inet_ntop(AF_INET, &p->dest.sin_addr, p->ip_str, sizeof(p->ip_str));
	return (0);
}
