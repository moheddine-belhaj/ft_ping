#include <stdio.h>
#include <math.h>
#include "ft_ping.h"

void	ft_stats_init(t_ping *p)
{
	p->rtt_min = 0;
	p->rtt_max = 0;
	p->rtt_sum = 0;
	p->rtt_sum_sq = 0;
	p->rtt_count = 0;
}

void	ft_stats_add_rtt(t_ping *p, double rtt_ms)
{
	if (p->rtt_count == 0 || rtt_ms < p->rtt_min)
		p->rtt_min = rtt_ms;
	if (p->rtt_count == 0 || rtt_ms > p->rtt_max)
		p->rtt_max = rtt_ms;
	p->rtt_sum += rtt_ms;
	p->rtt_sum_sq += rtt_ms * rtt_ms;
	p->rtt_count++;
}

/* Newton square root with the same coarse precision as inetutils: tiny
** variances (< prec) give 0, so the stddev prints as 0.000. */
static double	ft_nsqrt(double a, double prec)
{
	double	x0;
	double	x1;

	if (a < prec)
		return (0);
	x1 = a / 2;
	do
	{
		x0 = x1;
		x1 = (x0 + a / x0) / 2;
	}
	while (fabs(x1 - x0) > prec);
	return (x1);
}

void	ft_print_stats(const t_ping *p)
{
	double	loss = p->sent ? 100.0 * (p->sent - p->received) / p->sent : 0;
	double	avg;
	double	var;

	printf("--- %s ping statistics ---\n", p->target);
	printf("%lu packets transmitted, %lu packets received, %.0f%% packet loss\n",
		p->sent, p->received, loss);
	if (p->rtt_count > 0)
	{
		avg = p->rtt_sum / p->rtt_count;
		var = p->rtt_sum_sq / p->rtt_count - avg * avg;
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			p->rtt_min, avg, p->rtt_max, ft_nsqrt(var, 0.0005));
	}
}
