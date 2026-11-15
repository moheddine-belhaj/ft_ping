#include <string.h>
#include "ft_ping.h"

void	handle_sigint(int sig)
{
	(void)sig;
	g_stop = 1;
}

int	setup_signals(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	return (sigaction(SIGINT, &sa, NULL));
}

double	timeval_diff_ms(const struct timeval *a, const struct timeval *b)
{
	return ((a->tv_sec - b->tv_sec) * 1000.0 + (a->tv_usec - b->tv_usec) / 1000.0);
}
