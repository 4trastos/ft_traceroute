#include "ft_traceroute.h"

int init_struct(struct config *conf)
{
    conf->is_valid = false;
    conf->verbose_mode = false;
    conf->show_help = false;
    conf->hostname = NULL;
    conf->sockfd = -1;
    conf->sequence = 0;
    conf->ttl = 0;

    conf->stats.max_rtt = 0;
    conf->stats.min_rtt= 0;
}

int ft_parser(struct config *conf, char *argv, int argc)
{
    int i = 1;
    return (0);
}