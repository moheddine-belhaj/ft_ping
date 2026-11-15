#include <stdio.h>
#include <unistd.h>
#include "ft_ping.h"

/* TODO: send one packet per second, receive replies, update stats. */
int	ft_ping_loop(t_ping *p)
{
	gettimeofday(&p->start, NULL);
	printf("PING %s (%s): %d data bytes\n", p->target, p->ip_str, PING_DATA_LEN);
	return (0);
}
