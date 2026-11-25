#include <stdio.h>
#include <string.h>
#include "ft_ping.h"

#define USAGE_WIDTH 79

static const char	*g_help[] = {
	"Send ICMP ECHO_REQUEST packets to network hosts.\n\n",
	"      --ttl=N                specify N as time-to-live\n",
	"  -v, --verbose              verbose output\n\n",
	"  -?, --help                 give this help list\n",
	"      --usage                give a short usage message\n",
	NULL
};

static const char	*g_short[] = {
	"[-v?]", "[--ttl=N]", "[--verbose]", "[--help]", "[--usage]", "HOST ...",
	NULL
};

void	ft_print_usage(const char *prog)
{
	int	i;

	printf("Usage: %s [OPTION...] HOST ...\n", prog);
	for (i = 0; g_help[i]; i++)
		fputs(g_help[i], stdout);
}

/* "Usage: prog [opts] ..." wrapped like argp does, continuation aligned. */
void	ft_print_short_usage(const char *prog)
{
	size_t	indent = strlen("Usage: ") + strlen(prog) + 1;
	size_t	col;
	int		i;

	col = printf("Usage: %s", prog);
	for (i = 0; g_short[i]; i++)
	{
		if (col + 1 + strlen(g_short[i]) > USAGE_WIDTH)
		{
			printf("\n%*s%s", (int)indent, "", g_short[i]);
			col = indent + strlen(g_short[i]);
		}
		else
			col += printf(" %s", g_short[i]);
	}
	printf("\n");
}
