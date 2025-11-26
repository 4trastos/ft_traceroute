#include "ft_traceroute.h"

void    init_struct(struct config *conf)
{
    conf->is_valid = false;
    conf->show_version = false;
    conf->show_help = false;
    conf->timeout = false;
    conf->hostname = NULL;
    conf->sockfd = -1;
    conf->sequence = 0;
    conf->ttl = 64;
    conf->interface = NULL;
    conf->max_ttl = 30;
    conf->nprobes = 3;
    conf->tos = 0;
}

int ft_parser(struct config *conf, char **argv, int argc)
{
    int i = 1;
    int parser_result;

    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (ft_strcmp(argv[i], "-V") == 0 && conf->show_help == false)
                conf->show_version = true;
            else if (ft_strcmp(argv[i], "--help") == 0 && conf->show_version == false)
                conf->show_help = true;
            else if (ft_strcmp(argv[i], "-m") == 0 || (argv[i][0] == '-' && argv[i][1] == 'm' && argv[i][2] != '\0'))
            {
                parser_result = parser_max_ttl(conf, argv, argc, i);
                if (parser_result == -1)
                    return (-1);
                if (parser_result == 1)
                    i++;
            }
            else if (ft_strcmp(argv[i], "-q") == 0 || (argv[i][0] == '-' && argv[i][1] == 'q' && argv[i][2] != '\0'))
            {
                parser_result = parser_nprobes(conf, argv, argc, i);
                if (parser_result == -1)
                    return (-1);
                if (parser_result == 1)
                    i++;
            }
            else if (ft_strcmp(argv[i], "-i") == 0 || (argv[i][0] == '-' && argv[i][1] == 'i' && argv[i][2] != '\0'))
            {
                parser_result = parser_device(conf, argv, argc, i);
                if (parser_result == -1)
                    return (-1);
                if (parser_result == 1)
                    i++;
            }
            else if (ft_strcmp(argv[i], "-t") == 0 || (argv[i][0] == '-' && argv[i][1] == 't' && argv[i][2] != '\0'))
            {
                parser_result = parser_tos(conf, argv, argc, i);
                if (parser_result == -1)
                    return (-1);
                if (parser_result == 1)
                    i++;
            }
            else
            {
                printf("Bad option `%s' (argc %d) \n", argv[i], i);
                return (-1);
            }
        }
        else
        {
            if (conf->hostname != NULL)
            {
                printf("%s: Error: Only one hostname is allowed\n", argv[0]);
                return (-1);
            }
            conf->hostname = argv[i];
        }
        i++;
    }   
    if (conf->hostname == NULL && !conf->show_help && !conf->show_version)
    {
        printf("Specify \"host\" missing argument.\n");
        return (-1);
    }
    conf->is_valid = true;
    return (0);
}