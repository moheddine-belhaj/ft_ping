#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ft_ping.h"

static int	ft_send_echo(t_ping *p)
{
	unsigned char	pkt[PING_MAX_PACKET] __attribute__((aligned(8)));
	size_t			len;

	len = ft_build_packet(p, pkt);
	p->seq++;
	if (sendto(p->sockfd, pkt, len, 0,
			(struct sockaddr *)&p->dest, sizeof(p->dest)) < 0)
	{
		fflush(stdout);
		fprintf(stderr, "%s: sending packet: %s\n", p->prog, strerror(errno));
		close(p->sockfd);
		exit(1);
	}
	p->sent++;
	return (0);
}

/*
** Wait until deadline (or end, if set and earlier), handling any datagram that
** arrives. Returns 1 if the -w time limit was reached.
*/
static int	ft_wait_replies(t_ping *p, const struct timeval *deadline,
	const struct timeval *end)
{
	unsigned char	buf[PING_RECV_BUF] __attribute__((aligned(8)));
	struct pollfd	pfd = {p->sockfd, POLLIN, 0};
	struct timeval	now;
	double			left;
	ssize_t			n;

	while (!g_stop)
	{
		gettimeofday(&now, NULL);
		left = ft_timeval_diff_ms(deadline, &now);
		if (end && ft_timeval_diff_ms(end, &now) < left)
			left = ft_timeval_diff_ms(end, &now);
		if (left <= 0)
			return (end && ft_timeval_diff_ms(end, &now) <= 0);
		if (poll(&pfd, 1, (int)left + 1) <= 0)
			continue ;
		n = recvfrom(p->sockfd, buf, sizeof(buf), 0, NULL, NULL);
		if (n > 0)
			ft_parse_reply(p, buf, n);
	}
	return (0);
}

/* -w N: like inetutils, packets go out at 0..N seconds, then the stats. */
int	ft_ping_loop(t_ping *p)
{
	struct timeval	deadline;
	struct timeval	end;
	int				i;

	gettimeofday(&p->start, NULL);
	end = p->start;
	end.tv_sec += p->timeout;
	printf("PING %s (%s): %d data bytes", p->target, p->ip_str, p->size);
	if (p->verbose)
		printf(", id 0x%04x = %u", (unsigned short)p->id, (unsigned short)p->id);
	printf("\n");
	for (i = 0; i < p->preload && !g_stop; i++)
		ft_send_echo(p);
	do
	{
		gettimeofday(&deadline, NULL);
		deadline.tv_sec += PING_INTERVAL;
		ft_send_echo(p);
		if (ft_wait_replies(p, &deadline, p->timeout ? &end : NULL))
		{
			ft_send_echo(p);
			break ;
		}
	}
	while (!g_stop);
	return (0);
}
