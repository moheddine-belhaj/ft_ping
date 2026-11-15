#include <stdio.h>
#include <math.h>
#include "ft_ping.h"

void	stats_init(t_ping *p)
{
	p->rtt_min = 0;
	p->rtt_max = 0;
	p->rtt_sum = 0;
	p->rtt_sum_sq = 0;
}

void	stats_add_rtt(t_ping *p, double rtt_ms)
{
	if (p->received == 0 || rtt_ms < p->rtt_min)
		p->rtt_min = rtt_ms;
	if (p->received == 0 || rtt_ms > p->rtt_max)
		p->rtt_max = rtt_ms;
	p->rtt_sum += rtt_ms;
	p->rtt_sum_sq += rtt_ms * rtt_ms;
	p->received++;
}

void	print_stats(const t_ping *p)
{
	double	loss = p->sent ? 100.0 * (p->sent - p->received) / p->sent : 0;
	double	avg;
	double	var;

	printf("--- %s ping statistics ---\n", p->target);
	printf("%lu packets transmitted, %lu packets received, %.0f%% packet loss\n",
		p->sent, p->received, loss);
	if (p->received > 0)
	{
		avg = p->rtt_sum / p->received;
		var = p->rtt_sum_sq / p->received - avg * avg;
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			p->rtt_min, avg, p->rtt_max, var > 0 ? sqrt(var) : 0.0);
	}
}
