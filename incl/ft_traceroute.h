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


//*** Init Functions ***/

int         main(int argc, char **argv);

//*** Parser Logic***/

void        init_struct(struct config *conf);
void        show_help(void);
int         ft_parser(struct config *conf, char **argv, int argc);

//*** Signal Handler ***/

void        sigint_handler(int signum);
void        sigalrm_handler(int signum);
void        init_signal(void);

//*** Ping Logic ***/

int         dns_resolution(struct config *conf);
int         socket_creation(struct config *conf);
int         icmp_creation(struct config *conf);
uint16_t    calculate_checksum(void *packet, size_t len);
int         send_socket(struct config *conf);
int         receive_response(struct config *conf);
double      calculate_rtt(struct ping_packet *sent_packet);

//*** Statistics ***/

void        show_statistics(struct config *conf);
void        printf_verbose(struct config *conf);

#endif