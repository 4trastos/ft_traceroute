#include "ft_traceroute.h"

/* int socket_creation(struct config *conf)
{
    // Crear socket normalmente...
    conf->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    // Si se especificó una interfaz, enlazar a ella
    if (conf->device != NULL)
    {
        if (setsockopt(conf->sockfd, SOL_SOCKET, SO_BINDTODEVICE, 
                       conf->device, strlen(conf->device)) == -1)
        {
            printf("Cannot bind to device `%s`: %s\n", 
                   conf->device, strerror(errno));
            close(conf->sockfd);
            return (-1);
        }
    }
    return (0);
} */

/* int setup_socket_with_tos(struct config *conf)
{
    // Crear socket normalmente...
    conf->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    // Si se especificó TOS, aplicarlo
    if (conf->tos != 0)
    {
        if (setsockopt(conf->sockfd, IPPROTO_IP, IP_TOS, 
                       &conf->tos, sizeof(conf->tos)) == -1)
        {
            printf("Warning: Cannot set TOS %d: %s\n", 
                   conf->tos, strerror(errno));
            // Continuamos aunque falle (algunos sistemas requieren root)
        }
    }
    return (0);
} */

/* int setup_socket_with_interface(struct config *conf)
{
    conf->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    if (conf->interface != NULL)
    {
        if (setsockopt(conf->sockfd, SOL_SOCKET, SO_BINDTODEVICE, 
                       conf->interface, strlen(conf->interface)) == -1)
        {
            printf("setsockopt SO_BINDTODEVICE: %s\n", strerror(errno));
            close(conf->sockfd);
            return (-1);
        }
    }
    return (0);
} */