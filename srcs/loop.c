#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ft_ping.h"

static int	ft_send_echo(t_ping *p)
{
	unsigned char	pkt[PING_PACKET_LEN] __attribute__((aligned(8)));
	size_t			len;

	len = ft_build_packet(p, pkt);
	p->seq++;
	if (sendto(p->sockfd, pkt, len, 0,
			(struct sockaddr *)&p->dest, sizeof(p->dest)) < 0)
	{
		fflush(stdout);
		fprintf(stderr, "%s: sending packet: %s\n", p->prog, strerror(errno));
		return (-1);
	}
	p->sent++;
	return (0);
}

/* Wait up to the next send time, handling any datagram that arrives. */
static void	ft_wait_replies(t_ping *p, const struct timeval *deadline)
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
		if (left <= 0)
			return ;
		if (poll(&pfd, 1, (int)left + 1) <= 0)
			continue ;
		n = recvfrom(p->sockfd, buf, sizeof(buf), 0, NULL, NULL);
		if (n > 0)
			ft_parse_reply(p, buf, n);
	}
}

int	ft_ping_loop(t_ping *p)
{
	struct timeval	deadline;

	gettimeofday(&p->start, NULL);
	printf("PING %s (%s): %d data bytes", p->target, p->ip_str, PING_DATA_LEN);
	if (p->verbose)
		printf(", id 0x%04x = %u", (unsigned short)p->id, (unsigned short)p->id);
	printf("\n");
	while (!g_stop)
	{
		gettimeofday(&deadline, NULL);
		deadline.tv_sec += PING_INTERVAL;
		ft_send_echo(p);
		ft_wait_replies(p, &deadline);
	}
	return (0);
}
