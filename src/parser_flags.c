#include "ft_traceroute.h"

//  Interfaces válidas
// -i eth0      // Ethernet
// -i wlan0     // WiFi  
// -i lo        // Loopback
// -i tun0      // VPN
// -i br-123abc // Bridge

int parser_max_ttl(struct config *conf, char **argv, int argc, int i)
{
    char *arg_value = NULL;
    int is_separate = 0;

    if (ft_strcmp(argv[i], "-m") == 0)
    {
        if (i + 1 >= argc)
        {
            printf("Option `-m' (argc %d) requires an argument: `-m max_ttl'\n", i);
            return (-1);
        }
        arg_value = argv[i + 1];
        is_separate = 1;
    }
    else
    {
        arg_value = &argv[i][2];
        is_separate = 0;
    }
    
    if (!is_valid_number(arg_value))
    {
        printf("Cannot handle `-m' option with arg `%s' (argc %d)\n", arg_value, is_separate ? i + 1 : i);
        return (-1);
    }

    conf->max_ttl = ft_atoi(arg_value);
    if (conf->max_ttl <= 0 || conf->max_ttl >= 255)
    {
        printf("Invalid max TTL: %d. Must be between 1 and 255\n", conf->max_ttl);
        return (-1);
    }
    else
        return (is_separate);
}

int         parser_nprobes(struct config *conf, char **argv, int argc, int i)
{
    char *arg_value = NULL;
    int is_separate = 0;

    if (ft_strcmp(argv[i], "-q") == 0)
    {
        if (i + 1 >= argc)
        {
            printf("Option `-q' (argc %d) requires an argument: `-q nqueries'\n", i);
            return (-1);
        }
        arg_value = argv[i + 1];
        is_separate = 1;    
    }
    else
    {
        arg_value = &argv[i][2];
        is_separate = 0;
    }
    
    if (!is_valid_number(arg_value))
    {
        printf("Cannot handle `-q' option with arg `%s' (argc %d)\n", arg_value, is_separate ? i + 1 : i);
        return (-1);
    }

    conf->nprobes = ft_atoi(arg_value);
    if (conf->nprobes <= 0 || conf->nprobes > 10)
    {
        printf("no more than 10 probes per hop\n");
        return (-1);
    }
    else
        return (is_separate);
}

int         parser_device(struct config *conf, char **argv, int argc, int i)
{
    char *arg_value = NULL;
    int is_separate = 0;

    if (ft_strcmp(argv[i], "-i") == 0)
    {
        if (i + 1 >= argc)
        {
            printf("Option `-i' (argc %d) requires an argument: `-i device'\n", i);
            return (-1);
        }
        arg_value = argv[i + 1];
        is_separate = 1;    
    }
    else
    {
        arg_value = &argv[i][2];
        is_separate = 0;
    }

    if (!is_valid_interface_name(arg_value))
    {
        printf("Cannot handle `-i' option with arg `%s' (argc %d)\n", arg_value, is_separate ? i + 1 : i);
        return (-1);
    }

    conf->interface = arg_value;
    return (is_separate);
}

int         parser_tos(struct config *conf, char **argv, int argc, int i)
{
    char *arg_value = NULL;
    int is_separate = 0;

    if (ft_strcmp(argv[i], "-t") == 0)
    {
        if (i + 1 >= argc)
        {
            printf("Option `-t' (argc %d) requires an argument: `-t tos'\n", i);
            return (-1);
        }
        arg_value = argv[i + 1];
        is_separate = 1;
    }
    else
    {
        arg_value = &argv[i][2];
        is_separate = 0;
    }

    if (!is_valid_number(arg_value))
    {
        printf("Cannot handle `-t' option with arg `%s' (argc %d)\n", arg_value, is_separate ? i + 1 : i);
        return (-1);
    }

    conf->tos = ft_atoi(arg_value);
    if (conf->tos <= 0 || conf->tos >= 255)
    {
        printf("Invalid TOS: %d. Must be between 1 and 255\n", conf->tos);
        return (-1);
    }

    return (is_separate);
}