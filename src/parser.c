#include "ft_traceroute.h"

void    init_struct(struct config *conf)
{
    conf->is_valid = false;
    conf->show_version = false;
    conf->show_help = false;
    conf->max_ttl = true;
    conf->nprobes = false;
    conf->interval = false;
    conf->timeout = false;
    conf->hostname = NULL;
    conf->sockfd = -1;
    conf->sequence = 0;
    conf->ttl = 0;

    conf->stats.max_rtt = 0;
    conf->stats.min_rtt= 0;
    conf->stats.recieved_packets = 0;
    conf->stats.send_packets = 0;
    conf->stats.total_rtt= 0;
    conf->stats.total_rtt_sq = 0;
    conf->stats.total_ttl = 0;
    gettimeofday(&conf->stats.start_time, NULL);
}

int ft_parser(struct config *conf, char **argv, int argc)
{
    int i = 1;
    int x = 0;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (ft_strcmp(argv[i], "-V") == 0 && conf->show_help == false)
                conf->show_version = true;
            else if (ft_strcmp(argv[i], "--help") == 0 && conf->show_version == false)
                conf->show_help = true;
            else if (ft_strcmp(argv[i], "-m") == 0)
            {
                x = i;
                if (argv[i + 1] == NULL || (ft_atoi(argv[i + 1]) == 0))
                {
                    printf("Option `-m' (argc %d) requires an argument: `-m max_ttl'\n", x);
                    return (-1);
                }
                conf->max_ttl = false;
            }
            else if (ft_strcmp(argv[i], "-q") == 0)
            {
                x = i;
                if (argv[i++] == NULL || (ft_atoi(argv[i++]) == 0))
                {
                    printf("Option `-q' (argc %d) requires an argument: `-q nqueries'\n", x);
                    return (-1);
                }
                conf->nprobes = true;
            }
            else if (ft_strcmp(argv[i], "-i") == 0)
            {
                x = i;
                if (argv[i++] == NULL || (ft_atoi(argv[i++]) == 0))
                {
                    printf("Option `-i' (argc %d) requires an argument: `-i device'\n", x);
                    return (-1);
                }
                conf->interval = true;
            }
            else if (ft_strcmp(argv[i], "-t") == 0)
            {
                x = i;
                if (argv[i++] == NULL || (ft_atoi(argv[i++]) == 0))
                {
                    printf("Option `-t' (argc %d) requires an argument: `-t tos'\n", x);
                    return (-1);
                }
                conf->timeout = true;
            }
            else
            {
                if (!conf->show_help && !conf->show_version && !conf->max_ttl)
                {
                    printf("Bad option `%s' (argc %d) \n", argv[i], i);
                    return (-1);
                }
            }
        }
        else
        {
            if (conf->hostname != NULL)
            {
                printf("%s: Error: Only one hostname is allowed\n", argv[0]);
                return (-1);
            }
            if (conf->max_ttl == true || conf->nprobes == true || conf->interval == true || conf->timeout == true)
                i++;
            conf->hostname = argv[i];
        }
        i++;
    }   
    if (conf->hostname == NULL && !conf->show_help && !conf->show_version)
    {
        printf("%s: Error: Destination hostname required\n", argv[0]);
        return (-1);
    }
    conf->is_valid = true;
    return (0);
}