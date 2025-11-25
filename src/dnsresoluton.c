#include "ft_traceroute.h"

int dns_resolution(struct config *conf)
{
    struct addrinfo     hints;
    struct addrinfo     *result;
    struct addrinfo     *aux;
    struct sockaddr_in  *ipv4;
    int             status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;          // Cualquier socket type
    hints.ai_protocol = 0;          // Cualquier protocolo
    hints.ai_flags = 0; 

    status = getaddrinfo(conf->hostname, NULL, &hints, &result);
    if (status != 0)
    {
        printf("❌ Error: Obtaining the host: ( %s ) ❌\n", gai_strerror(status));
        freeaddrinfo(result);
        return (-1);
    }

    aux = result;
    while (aux != NULL)
    {
        if (aux->ai_family == AF_INET)
        {
            ipv4 = (struct sockaddr_in *)aux->ai_addr; 
            conf->ip_address = ipv4->sin_addr;
            freeaddrinfo(result);
            return (0);
        }
        aux = aux->ai_next;
    }
    
    printf("❌ Error: no IPv4 address found\n");
    freeaddrinfo(result);
    return (-1);
}