#ifndef FT_PING_H
# define FT_PING_H

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <signal.h>
# include <stddef.h>
# include <sys/time.h>

# define PING_DATA_LEN   56
# define PING_INTERVAL   1
# define PING_TTL_DEFAULT 64
# define PING_ICMP_HDR_LEN 8
# define PING_PACKET_LEN (PING_ICMP_HDR_LEN + PING_DATA_LEN)
# define PING_RECV_BUF  1500

typedef struct s_ping
{
	const char			*prog;
	char				**hosts;     /* operands, compacted in argv */
	int					nhosts;
	const char			*target;     /* current host as typed by the user */
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

/* args.c, usage.c */
int				ft_parse_args(t_ping *p, int argc, char **argv);
void			ft_print_usage(const char *prog);
void			ft_print_short_usage(const char *prog);

/* utils.c */
const char		*ft_prog_name(const char *argv0);

/* resolve.c */
int				ft_resolve_target(t_ping *p);

/* socket.c */
int				ft_open_socket(t_ping *p);

/* packet.c */
size_t			ft_build_packet(t_ping *p, unsigned char *buf);
int				ft_parse_reply(t_ping *p, unsigned char *buf, ssize_t len);

/* icmp_error.c */
int				ft_handle_error(t_ping *p, const struct iphdr *ip,
					const struct icmphdr *icmp, size_t icmp_len);

/* checksum.c */
unsigned short	ft_checksum(const void *data, size_t len);

/* loop.c */
int				ft_ping_loop(t_ping *p);

/* stats.c */
void			ft_stats_init(t_ping *p);
void			ft_stats_add_rtt(t_ping *p, double rtt_ms);
void			ft_print_stats(const t_ping *p);

/* utils.c (signals, time) */
void			ft_handle_sigint(int sig);
int				ft_setup_signals(void);
double			ft_timeval_diff_ms(const struct timeval *a, const struct timeval *b);

#endif
