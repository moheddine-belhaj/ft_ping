#include <stdio.h>
#include <unistd.h>
#include "ft_ping.h"

volatile sig_atomic_t	g_stop = 0;

/* Ping one host; returns the exit status for it. */
static int	ft_ping_host(t_ping *p, const char *host)
{
	p->target = host;
	p->sent = 0;
	p->received = 0;
	p->seq = 0;
	if (ft_resolve_target(p) != 0)
		return (-1);
	ft_stats_init(p);
	ft_ping_loop(p);
	ft_print_stats(p);
	return (p->received > 0 ? 0 : 1);
}

int	main(int argc, char **argv)
{
	t_ping	p = {0};
	int		status = 0;
	int		i;

	p.prog = ft_prog_name(argv[0]);
	p.ttl = PING_TTL_DEFAULT;
	p.id = getpid();
	p.sockfd = -1;
	status = ft_parse_args(&p, argc, argv);
	if (status != 0)
		return (status);
	if (ft_open_socket(&p) != 0)
		return (1);
	if (ft_setup_signals() != 0)
		return (1);
	for (i = 0; i < p.nhosts; i++)
	{
		status = ft_ping_host(&p, p.hosts[i]);
		if (status < 0)
			break ;
	}
	close(p.sockfd);
	return (status < 0 ? 1 : status);
}
