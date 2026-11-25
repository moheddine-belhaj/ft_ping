#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "ft_ping.h"

int	ft_open_socket(t_ping *p)
{
	struct timeval	tv = {1, 0};

	p->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (p->sockfd < 0)
	{
		if (errno == EPERM || errno == EACCES)
			fprintf(stderr, "%s: Lacking privilege for raw socket.\n", p->prog);
		else
			fprintf(stderr, "%s: socket: %s\n", p->prog, strerror(errno));
		return (-1);
	}
	if (setsockopt(p->sockfd, IPPROTO_IP, IP_TTL, &p->ttl, sizeof(p->ttl)) < 0
		|| setsockopt(p->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		fprintf(stderr, "%s: setsockopt: %s\n", p->prog, strerror(errno));
		return (-1);
	}
	return (0);
}
