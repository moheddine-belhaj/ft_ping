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
	if (parse_args(&p, argc, argv) != 0)
		return (64);
	if (resolve_target(&p) != 0)
		return (1);
	if (open_socket(&p) != 0)
		return (1);
	if (setup_signals() != 0)
		return (1);
	stats_init(&p);
	ping_loop(&p);
	print_stats(&p);
	close(p.sockfd);
	return (p.received > 0 ? 0 : 1);
}
