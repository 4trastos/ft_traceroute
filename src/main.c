#include "ft_traceroute.h"

volatile sig_atomic_t g_sigint_received = 0;

void    cleanup(struct config *conf)
{
    if (conf->sockfd != -1)
        close(conf->sockfd);
    free(conf);
}

int main(int argc, char **argv)
{
    struct config   *conf;
    int exit = 0;
    unsigned char   *bytes;

    if (argc < 2)
    {
        show_help();
        return (1);
    }
    conf = malloc(sizeof(struct config));
    if (!conf)
        return (1);

    init_signal();
    init_struct(conf);
    conf->argc = argc;
    if (ft_parser(conf, argv, argc) != 0)
        exit = 1;
    else if (conf->is_valid && (conf->show_help || conf->show_version))
    {
        show_options(conf);
        exit = 0;
    }
    else if (dns_resolution(conf) != 0)
        exit = 1;
    else if (socket_creation(conf) != 0)
        exit = 1;
    else
    {
        bytes = (unsigned char *)&conf->ip_address;
        printf("traceroute to %s : (%d.%d.%d.%d), %d hops max, %d byte packets\n", 
            conf->hostname, bytes[0], bytes[1], bytes[2], bytes[3], conf->max_ttl, conf->packet_size);
        if (send_socket(conf) != 0)
            exit = 1;
    }

    cleanup(conf);
    return (exit);
}