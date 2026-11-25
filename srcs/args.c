#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ft_ping.h"

#define EXIT_USAGE 64

static int	ft_bad_option(const char *prog, const char *fmt, const char *arg)
{
	fprintf(stderr, "%s: ", prog);
	fprintf(stderr, fmt, arg);
	fprintf(stderr, "Try '%s --help' or '%s --usage' for more information.\n",
		prog, prog);
	return (-EXIT_USAGE);
}

/* --ttl N or --ttl=N */
static int	ft_parse_ttl(t_ping *p, char **argv, int *i)
{
	const char	*val = NULL;
	char		*end;
	unsigned long	n;

	if (argv[*i][5] == '=')
		val = argv[*i] + 6;
	else if (argv[*i + 1])
		val = argv[++(*i)];
	if (!val)
		return (ft_bad_option(p->prog, "option '--ttl' requires an argument\n",
				""));
	n = strtoul(val, &end, 10);
	if (*end)
		fprintf(stderr, "%s: invalid value (`%s' near `%s')\n", p->prog, val, end);
	else if (n < 1)
		fprintf(stderr, "%s: option value too small: %s\n", p->prog, val);
	else if (n > 255)
		fprintf(stderr, "%s: option value too big: %s\n", p->prog, val);
	if (*end || n < 1 || n > 255)
		return (-1);
	p->ttl = (int)n;
	return (0);
}

static int	ft_parse_long(t_ping *p, char **argv, int *i)
{
	const char	*a = argv[*i];

	if (!strcmp(a, "--verbose"))
		p->verbose = 1;
	else if (!strcmp(a, "--ttl") || !strncmp(a, "--ttl=", 6))
		return (ft_parse_ttl(p, argv, i));
	else if (!strcmp(a, "--help"))
	{
		ft_print_usage(p->prog);
		exit(0);
	}
	else if (!strcmp(a, "--usage"))
	{
		ft_print_short_usage(p->prog);
		exit(0);
	}
	else
		return (ft_bad_option(p->prog, "unrecognized option '%s'\n", a));
	return (0);
}

/* Short options, possibly grouped (-vv). */
static int	ft_parse_short(t_ping *p, const char *a)
{
	char	bad[2];

	for (a++; *a; a++)
	{
		if (*a == 'v')
			p->verbose = 1;
		else if (*a == '?')
		{
			ft_print_usage(p->prog);
			exit(0);
		}
		else
		{
			bad[0] = *a;
			bad[1] = '\0';
			return (ft_bad_option(p->prog, "invalid option -- '%s'\n", bad));
		}
	}
	return (0);
}

/* Returns -1 on error, 0 if argv[*i] was an option, 1 if it is an operand. */
static int	ft_parse_one(t_ping *p, char **argv, int *i, int opts)
{
	const char	*a = argv[*i];

	if (!opts || a[0] != '-' || !a[1])
		return (1);
	if (a[1] == '-')
		return (ft_parse_long(p, argv, i));
	return (ft_parse_short(p, a));
}

/* Options are consumed; the operands are compacted into p->hosts. */
int	ft_parse_args(t_ping *p, int argc, char **argv)
{
	int	i;
	int	opts = 1;
	int	ret;

	p->hosts = argv + 1;
	for (i = 1; i < argc; i++)
	{
		if (opts && !strcmp(argv[i], "--"))
		{
			opts = 0;
			continue ;
		}
		ret = ft_parse_one(p, argv, &i, opts);
		if (ret < 0)
			return (-ret);
		if (ret > 0)
			p->hosts[p->nhosts++] = argv[i];
	}
	if (!p->nhosts)
		return (-ft_bad_option(p->prog, "missing host operand\n", ""));
	return (0);
}
