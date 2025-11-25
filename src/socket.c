#include "ft_traceroute.h"

int socket_creation(struct config *conf)
{
    struct timeval timeout = {4, 0};

    conf->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (conf->sockfd == -1)
    {
        printf("ft_traceroute: socket error: %s\n", strerror(errno));
        return -1;
    }

    // Timeout de recepción
    if (setsockopt(conf->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
    {
        printf("ft_traceroute: setsockopt (SO_RCVTIMEO) error: %s\n", strerror(errno));
        close(conf->sockfd);
        return -1;
    }

#ifdef SO_BINDTODEVICE
    if (conf->interface != NULL)
    {
        if (setsockopt(conf->sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                       conf->interface, ft_strlen(conf->interface) + 1) == -1)
        {
            printf("ft_traceroute: setsockopt (SO_BINDTODEVICE) error: %s\n", strerror(errno));
            close(conf->sockfd);
            return -1;
        }
    }
#endif
    return 0;
}
