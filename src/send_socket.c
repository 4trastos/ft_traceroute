#include "ft_traceroute.h"

int         send_socket(struct config *conf)
{
    struct sockaddr_in  dest;
    struct sockaddr_in  recv_addr;
    socklen_t           addr_len;
    char                buffer[512];
    ssize_t             sent_bytes;
    ssize_t             recv_bytes;
    int                 port;
    int                 ttl;
    int                 recv_sock;

    port = 33434;
    addr_len = sizeof(recv_addr);

    if (conf->sockfd == -1 || conf->hostname == NULL)
        return (-1);

    // Socket RAW para recibir ICMP
    recv_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recv_sock < 0)
    {
        printf("ft_traceroute: socket raw ICMP failed: %s\n", strerror(errno));
        return (-1);
    }

    // Timeout de recepción
    struct timeval timeout = {1, 0};
    setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    for (ttl = 1; ttl <= conf->max_ttl && !g_sigint_received; ttl++)
    {
        if (setsockopt(conf->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
        {
            printf("ft_traceroute: setsockopt TTL %d failed: %s\n", ttl, strerror(errno));
            close(recv_sock);
            return (-1);
        }

        printf("%2d  ", ttl);

        for (int probe = 0; probe < conf->nprobes; probe++)
        {
            ft_memset(&dest, 0, sizeof(dest));
            dest.sin_family = AF_INET;
            dest.sin_port = htons(port);
            dest.sin_addr = conf->ip_address;

            struct timeval start;
            struct timeval end;
            gettimeofday(&start, NULL);

            // enviar UDP
            sent_bytes = sendto(conf->sockfd, "X", 1, 0, (struct sockaddr *)&dest, sizeof(dest));
            if (sent_bytes < 0)
            {
                printf("* ");
                continue;;
            }

            // recibir ICMP
            recv_bytes = recvfrom(recv_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
            gettimeofday(&end, NULL);
            if (recv_bytes < 0)
            {
                printf("* ");
                continue;
            }

            // RTT
            double rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

            // Obtener IP y nombre de host
            unsigned char *ip = (unsigned char *)&recv_addr.sin_addr.s_addr;
            char    host[NI_MAXHOST];
            if (getnameinfo((struct sockaddr *)&recv_addr, sizeof(recv_addr), host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
                printf("%s (%d.%d.%d.%d) %.3f ms  ", host, ip[0], ip[1], ip[2], ip[3], rtt);
            else
                printf("%d.%d.%d.%d (%d.%d.%d.%d) %.3f ms  ", ip[0], ip[1], ip[2], ip[3], ip[0], ip[1], ip[2], ip[3], rtt);
            
            // verificar si destino alcanzado
            struct iphdr    *ip_hdr = (struct iphdr *)buffer;
            struct icmphdr  *icmp_hdr = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);
            if (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)
            {
                ttl = conf->max_ttl;
                break;
            }
        }

        printf("\n");
        
    }
    close(recv_sock);
    return (0);
}