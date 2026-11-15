#include <stdio.h>
#include <unistd.h>
#include "ft_ping.h"

volatile sig_atomic_t	g_stop = 0;

int	main(int argc, char **argv)
{
	t_ping	p = {0};

	p.prog = argv[0];
	p.ttl = PING_TTL_DEFAULT;
	p.id = getpid();
	p.sockfd = -1;
	if (ft_parse_args(&p, argc, argv) != 0)
		return (64);
	if (ft_resolve_target(&p) != 0)
		return (1);
	if (ft_open_socket(&p) != 0)
		return (1);
	if (ft_setup_signals() != 0)
		return (1);
	ft_stats_init(&p);
	ft_ping_loop(&p);
	ft_print_stats(&p);
	close(p.sockfd);
	return (p.received > 0 ? 0 : 1);
}
