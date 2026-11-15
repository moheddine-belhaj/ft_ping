#ifndef FT_PING_H
# define FT_PING_H

# include <netinet/in.h>
# include <signal.h>
# include <stddef.h>
# include <sys/time.h>

# define PING_DATA_LEN   56
# define PING_INTERVAL   1
# define PING_TTL_DEFAULT 64

typedef struct s_ping
{
	const char			*prog;
	const char			*target;     /* argument as typed by the user */
	char				ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in	dest;
	int					sockfd;
	int					verbose;
	int					ttl;
	pid_t				id;          /* ICMP identifier (pid) */
	unsigned short		seq;
	/* statistics */
	unsigned long		sent;
	unsigned long		received;
	double				rtt_min;
	double				rtt_max;
	double				rtt_sum;
	double				rtt_sum_sq;
	struct timeval		start;
}	t_ping;

extern volatile sig_atomic_t	g_stop;

/* args.c */
int				ft_parse_args(t_ping *p, int argc, char **argv);
void			ft_print_usage(const char *prog);

/* resolve.c */
int				ft_resolve_target(t_ping *p);

/* socket.c */
int				ft_open_socket(t_ping *p);

/* packet.c */
size_t			ft_build_packet(t_ping *p, unsigned char *buf);
int				ft_parse_reply(t_ping *p, unsigned char *buf, ssize_t len);

/* checksum.c */
unsigned short	ft_checksum(const void *data, size_t len);

/* loop.c */
int				ft_ping_loop(t_ping *p);

/* stats.c */
void			ft_stats_init(t_ping *p);
void			ft_stats_add_rtt(t_ping *p, double rtt_ms);
void			ft_print_stats(const t_ping *p);

/* utils.c */
void			ft_handle_sigint(int sig);
int				ft_setup_signals(void);
double			ft_timeval_diff_ms(const struct timeval *a, const struct timeval *b);

#endif
