#include "ft_traceroute.h"

int validate_icmp(unsigned char *buf, ssize_t len, struct in_addr dest_ip, uint16_t expected_port)
{
    if (len < (ssize_t)sizeof(struct iphdr))
        return (0);

    struct iphdr *ip = (struct iphdr *)buf;
    if (len < (ssize_t)(ip->ihl * 4 + sizeof(struct icmphdr)))
        return (0);

    struct icmphdr *icmp = (struct icmphdr *)(buf + ip->ihl * 4);

    if (icmp->type != ICMP_TIME_EXCEEDED && !(icmp->type == ICMP_DEST_UNREACH && icmp->code == ICMP_PORT_UNREACH))
        return (0);

    // Paquete original dentro del ICMP
    unsigned char *inner = buf + ip->ihl * 4 + sizeof(struct icmphdr);
    if ((unsigned char *)inner + sizeof(struct iphdr) > buf + len)
        return (0);

    struct iphdr *inner_ip = (struct iphdr *)inner;
    if ((unsigned char *)inner + inner_ip->ihl * 4 + 8 > buf + len)
        return (0);

    struct udphdr *inner_udp = (struct udphdr *)(inner + inner_ip->ihl * 4);

    // Verifica que sea para nuestro host y puerto esperado
    if (inner_ip->daddr != dest_ip.s_addr)
        return (0);

    return (ntohs(inner_udp->dest) == expected_port);
}

int         send_socket(struct config *conf)
{
    struct sockaddr_in  dest;
    struct sockaddr_in  recv_addr;
    socklen_t           addr_len_recv;
    unsigned char       buffer[512];
    ssize_t             sent_bytes;
    ssize_t             recv_bytes;
    int                 port;
    int                 ttl;
    int                 recv_sock;
    uint16_t            dest_port;

    port = 33434;
    addr_len_recv = sizeof(recv_addr);

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
    struct timeval timeout = {3, 0};
    setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    for (ttl = 1; ttl <= conf->max_ttl && !g_sigint_received; ttl++)
    {
        in_addr_t last_ip = 0;

        if (setsockopt(conf->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
        {
            printf("ft_traceroute: setsockopt TTL %d failed: %s\n", ttl, strerror(errno));
            close(recv_sock);
            return (-1);
        }

        printf("%2d  ", ttl);

        for (int probe = 0; probe < conf->nprobes && !g_sigint_received; probe++)
        {
            ft_memset(&dest, 0, sizeof(dest));
            dest.sin_family = AF_INET;
            dest_port = port + ttl + probe;
            dest.sin_port = htons(dest_port);
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
            addr_len_recv = sizeof(recv_addr);
            recv_bytes = recvfrom(recv_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len_recv);
            gettimeofday(&end, NULL);
            if (recv_bytes < 0)
            {
                printf("* ");
                continue;
            }

            if (!validate_icmp(buffer, recv_bytes, conf->ip_address, dest_port))
            {
                printf("* ");
                continue;
            }

            // RTT
            double rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

            in_addr_t current_ip = recv_addr.sin_addr.s_addr;

            // Obtener IP y nombre de host
            unsigned char *ip_bytes = (unsigned char *)&current_ip;
            char    host[NI_MAXHOST];

            // Solo imprimir información completa del host si la IP ha cambiado con respecto a la última respuesta OK
            if (current_ip != last_ip)
            {
                // Es un router nuevo, imprimir info completa
                if (getnameinfo((struct sockaddr *)&recv_addr, sizeof(recv_addr), host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
                    printf("%s (%d.%d.%d.%d) %.3f ms  ", host, ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], rtt);
                else
                    printf("%d.%d.%d.%d (%d.%d.%d.%d) %.3f ms  ", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], rtt);
                last_ip = current_ip;
            }
            else
                printf("%.3f ms  ", rtt);
            
            // verificar si destino alcanzado
            struct iphdr *ip_hdr = (struct iphdr *)buffer;
            struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);

            if (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)
            {
                printf("\n");
                close(recv_sock);
                return 0;
            }

            /* struct iphdr    *ip_hdr = (struct iphdr *)buffer;
            if ((ip_hdr->ihl * 4) < recv_bytes)
            {
                struct icmphdr  *icmp_hdr = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);
                if (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)
                {
                    ttl = conf->max_ttl;
                    break;
                }
            } */
        }

        printf("\n");
        
    }
    close(recv_sock);
    return (0);
}