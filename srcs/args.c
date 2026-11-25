#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ft_ping.h"

#define EXIT_USAGE 64

typedef enum e_opt_id
{
	OPT_VERBOSE, OPT_HELP, OPT_USAGE, OPT_TTL, OPT_NUMERIC,
	OPT_TIMEOUT, OPT_LINGER, OPT_SIZE
}	t_opt_id;

typedef struct s_opt
{
	const char	*name;		/* long name, without the leading -- */
	char		letter;		/* short name, 0 if none */
	int			has_arg;
	t_opt_id	id;
}	t_opt;

static const t_opt	g_opts[] = {
	{"verbose", 'v', 0, OPT_VERBOSE},
	{"help", '?', 0, OPT_HELP},
	{"usage", 0, 0, OPT_USAGE},
	{"ttl", 0, 1, OPT_TTL},
	{"numeric", 'n', 0, OPT_NUMERIC},
	{"timeout", 'w', 1, OPT_TIMEOUT},
	{"linger", 'W', 1, OPT_LINGER},
	{"size", 's', 1, OPT_SIZE},
	{NULL, 0, 0, 0}
};

static int	ft_bad_option(const char *prog, const char *fmt, const char *arg)
{
	fprintf(stderr, "%s: ", prog);
	fprintf(stderr, fmt, arg);
	fprintf(stderr, "Try '%s --help' or '%s --usage' for more information.\n",
		prog, prog);
	return (-EXIT_USAGE);
}

/* Numeric argument in [min, max]; same messages as inetutils. -1 on error. */
static int	ft_parse_num(const t_ping *p, const char *val, unsigned long max,
	unsigned long min)
{
	unsigned long	n;
	char			*end;

	n = strtoul(val, &end, 10);
	if (*end)
		fprintf(stderr, "%s: invalid value (`%s' near `%s')\n", p->prog, val, end);
	else if (n < min)
		fprintf(stderr, "%s: option value too small: %s\n", p->prog, val);
	else if (n > max)
		fprintf(stderr, "%s: option value too big: %s\n", p->prog, val);
	if (*end || n < min || n > max)
		return (-1);
	return ((int)n);
}

static int	ft_set_num(t_ping *p, t_opt_id id, const char *val)
{
	int	n;

	if (id == OPT_TTL)
		n = ft_parse_num(p, val, 255, 1);
	else if (id == OPT_SIZE)
		n = ft_parse_num(p, val, PING_MAX_DATA, 0);
	else
		n = ft_parse_num(p, val, INT_MAX, 1);
	if (n < 0)
		return (-1);
	if (id == OPT_TTL)
		p->ttl = n;
	else if (id == OPT_SIZE)
		p->size = n;
	else if (id == OPT_TIMEOUT)
		p->timeout = n;
	else
		p->linger = n;
	return (0);
}

/* Applies one parsed option; val is NULL for options without argument. */
static int	ft_apply(t_ping *p, t_opt_id id, const char *val)
{
	if (id == OPT_VERBOSE)
		p->verbose = 1;
	else if (id == OPT_NUMERIC)
		p->numeric = 1;
	else if (id == OPT_HELP || id == OPT_USAGE)
	{
		if (id == OPT_HELP)
			ft_print_usage(p->prog);
		else
			ft_print_short_usage(p->prog);
		exit(0);
	}
	else
		return (ft_set_num(p, id, val));
	return (0);
}

/* --name, --name=value or --name value */
static int	ft_parse_long(t_ping *p, char **argv, int *i)
{
	const char	*a = argv[*i] + 2;
	size_t		len = strcspn(a, "=");
	const char	*val = a[len] ? a + len + 1 : NULL;
	const t_opt	*o;

	for (o = g_opts; o->name; o++)
		if (strlen(o->name) == len && !strncmp(o->name, a, len))
			break ;
	if (!o->name)
		return (ft_bad_option(p->prog, "unrecognized option '%s'\n", argv[*i]));
	if (!o->has_arg && val)
		return (ft_bad_option(p->prog, "option '--%s' doesn't allow an argument\n",
				o->name));
	if (o->has_arg && !val && argv[*i + 1])
		val = argv[++(*i)];
	if (o->has_arg && !val)
		return (ft_bad_option(p->prog, "option '--%s' requires an argument\n",
				o->name));
	return (ft_apply(p, o->id, val));
}

/* Short options, possibly grouped (-vn) or with an attached value (-s56). */
static int	ft_parse_short(t_ping *p, char **argv, int *i)
{
	const char	*a;
	const t_opt	*o;
	char		bad[2];
	int			ret = 0;

	for (a = argv[*i] + 1; *a; a++)
	{
		for (o = g_opts; o->name; o++)
			if (o->letter == *a)
				break ;
		bad[0] = *a;
		bad[1] = '\0';
		if (!o->name)
			return (ft_bad_option(p->prog, "invalid option -- '%s'\n", bad));
		if (!o->has_arg)
			ret = ft_apply(p, o->id, NULL);
		else if (a[1] || argv[*i + 1])
			return (ft_apply(p, o->id, a[1] ? a + 1 : argv[++(*i)]));
		else
			return (ft_bad_option(p->prog, "option requires an argument -- '%s'\n",
					bad));
		if (ret != 0)
			return (ret);
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
	return (ft_parse_short(p, argv, i));
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
