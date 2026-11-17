#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ft_ping.h"

void	ft_print_usage(const char *prog)
{
	printf("Usage: %s [OPTION...] HOST ...\n", prog);
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -v, --verbose              verbose output\n");
	printf("      --ttl=N                specify N as time-to-live (1-255)\n");
	printf("  -?, --help                 give this help list\n");
}

/* --ttl N or --ttl=N */
static int	ft_parse_ttl(t_ping *p, char **argv, int *i)
{
	const char	*val = NULL;
	char		*end;
	long		n;

	if (argv[*i][5] == '=')
		val = argv[*i] + 6;
	else if (argv[*i][5] == '\0' && argv[*i + 1])
		val = argv[++(*i)];
	if (!val || !*val)
	{
		fprintf(stderr, "%s: option '--ttl' requires an argument\n", argv[0]);
		return (-1);
	}
	n = strtol(val, &end, 10);
	if (*end || n < 1 || n > 255)
	{
		fprintf(stderr, "%s: invalid value (`%s' near `%s')\n", argv[0], val, end);
		return (-1);
	}
	p->ttl = (int)n;
	return (0);
}

static int	ft_parse_option(t_ping *p, char **argv, int *i)
{
	if (!strcmp(argv[*i], "-v"))
		p->verbose = 1;
	else if (!strncmp(argv[*i], "--ttl", 5))
		return (ft_parse_ttl(p, argv, i));
	else if (!strcmp(argv[*i], "-?"))
	{
		ft_print_usage(argv[0]);
		exit(0);
	}
	else
	{
		fprintf(stderr, "%s: invalid option -- '%s'\n", argv[0], argv[*i] + 1);
		fprintf(stderr, "Try '%s -?' for more information.\n", argv[0]);
		return (-1);
	}
	return (0);
}

int	ft_parse_args(t_ping *p, int argc, char **argv)
{
	int	i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			if (ft_parse_option(p, argv, &i) != 0)
				return (-1);
		}
		else if (!p->target)
			p->target = argv[i];
	}
	if (!p->target)
	{
		fprintf(stderr, "%s: missing host operand\n", argv[0]);
		fprintf(stderr, "Try '%s -?' for more information.\n", argv[0]);
		return (-1);
	}
	return (0);
}
