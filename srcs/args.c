#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ft_ping.h"

void	print_usage(const char *prog)
{
	printf("Usage: %s [OPTION...] HOST ...\n", prog);
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -v, --verbose              verbose output\n");
	printf("  -?, --help                 give this help list\n");
}

int	parse_args(t_ping *p, int argc, char **argv)
{
	int	i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			if (!strcmp(argv[i], "-v"))
				p->verbose = 1;
			else if (!strcmp(argv[i], "-?"))
				return (print_usage(argv[0]), exit(0), 0);
			else
			{
				fprintf(stderr, "%s: invalid option -- '%s'\n", argv[0], argv[i] + 1);
				fprintf(stderr, "Try '%s -?' for more information.\n", argv[0]);
				return (-1);
			}
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
