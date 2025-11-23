#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <signal.h>
# include <stdbool.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>
# include <math.h>

# define MAX_PACKETS 1024
# define ICMP_PAYLOAD_SIZE 56
# define MAX_TTL 30

extern volatile sig_atomic_t g_sigint_received;

struct stats
{
    int                 send_packets;
    int                 recieved_packets;
    int                 total_ttl;
    double              min_rtt;
    double              max_rtt;
    double              total_rtt;
    double              total_rtt_sq;
    struct timeval      start_time;
};

struct ping_packet
{
    struct icmphdr      icmp_header;
    struct timeval      timestamp; 
    char                data[ICMP_PAYLOAD_SIZE];
};

struct config
{
    char                *hostname;
    char                *interface;
    bool                show_version;
    bool                show_help;
    bool                is_valid;
    bool                timeout;
    int                 nprobes;
    int                 sockfd;
    int                 ttl;
    int                 max_ttl;
    int                 tos;
    int                 argc;
    uint16_t            sequence;
    struct in_addr      ip_address;
    struct stats        stats;
    struct ping_packet  packets[MAX_PACKETS]; 
};

//*** Init Functions ***/

int         main(int argc, char **argv);

//*** Auxliar Functions ***/

int         ft_strcmp(const char *s1, const char *s2);
int         ft_atoi(char *str);
int         ft_isdigit(int c);
int         is_valid_number(char *str);
int         is_valid_interface_name(char *name);
int         ft_isalnum(int c);

//*** Parser Logic***/

void        init_struct(struct config *conf);
void        show_options(struct config *conf);
void        show_help(void);
void        show_version(void);
int         ft_parser(struct config *conf, char **argv, int argc);
int         parser_max_ttl(struct config *conf, char **argv, int argc, int i);
int         parser_nprobes(struct config *conf, char **argv, int argc, int i);
int         parser_device(struct config *conf, char **argv, int argc, int i);
int         parser_tos(struct config *conf, char **argv, int argc, int i);

//*** Signal Handler ***/

void        sigint_handler(int signum);
void        init_signal(void);
//void        sigalrm_handler(int signum);

//*** Ping Logic ***/

// int         dns_resolution(struct config *conf);
// int         socket_creation(struct config *conf);
// int         icmp_creation(struct config *conf);
// uint16_t    calculate_checksum(void *packet, size_t len);
// int         send_socket(struct config *conf);
// int         receive_response(struct config *conf);
// double      calculate_rtt(struct ping_packet *sent_packet);

//*** Statistics ***/

// void        show_statistics(struct config *conf);
// void        printf_verbose(struct config *conf);

#endif