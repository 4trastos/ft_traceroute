#include "ft_traceroute.h"

void    show_options(struct config *conf)
{
    if (conf->show_help == true)
        show_help();
    else
        show_version();
    return;
}

void    show_version(void)
{
    printf("Modern traceroute for Linux, version 2.1.5\n");
    printf("Copyright (c) 2016  Dmitry Butskoy,   License: GPL v2 or any later\n");
}

void    show_help(void)
{
    printf("Usage:\n");
    printf("./ft_traceroute [ -i device ] [ -m max_ttl ] [ -t tos ] [ -q nqueries ]\n");
    printf("Options:\n");
    printf("-i device  --interface=device\n");
    printf("                            Specify a network interface to operate with\n");
    printf("-m max_ttl  --max-hops=max_ttl\n");
    printf("                            Set the max number of hops (max TTL to be\n");
    printf("                            reached). Default is 30\n");
    printf("-t tos  --tos=tos           Set the TOS (IPv4 type of service) or TC (IPv6\n");
    printf("                            traffic class) value for outgoing packets\n");
    printf("-q nqueries  --queries=nqueries\n");
    printf("                            Set the number of probes per each hop. Default is\n");
    printf("                            3\n");
    printf("-V  --version               Print version info and exit\n");
    printf("--help                      Read this help and exit\n");
    printf("\nArguments:\n");
    printf("+     host          The host to traceroute to\n      packetlen     The full packet length (default is the length of an IP\n");
    printf("                    header plus 40). Can be ignored or increased to a minimal\n");
    printf("                    allowed value\n");
}