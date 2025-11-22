#include "ft_traceroute.h"

int parser_max_ttl(char **argv, int i, int x)
{
    if (argv[i + 1] == NULL)
    {
        printf("Option `-m' (argc %d) requires an argument: `-m max_ttl'\n", x);
        return (-1);
    }
    else if ((ft_atoi(argv[i + 1]) == 0))
    {
        printf("Cannot handle `-m' option with arg `%s' (argc %d)\n", argv[i + 1], x + 1);
        return (-1);
    }
    else if (argv[i][2] != 0 && argv[i + 1] == NULL)
    {
        char str[4] = "host";
        printf("Specify %s missing argument.\n", str);
        return (-1);
    }
    else
        return (0);
}