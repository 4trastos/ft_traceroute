#include "ft_traceroute.h"

volatile sig_atomic_t g_sigint_received = 0;

int main(int argc, char **argv)
{
    struct config   *conf;
    int exit = 0;

    if (argc < 2)
    {
        printf("%s: usage error: Destination address required \n", argv[0]);
        return (1);
    }
    conf = malloc(sizeof(struct config));
    if (!conf)
        return (1);

    init_signal();
    if (ft_parser(conf, argv, argc) != 0)
        exit = 1;
    if (conf->is_valid && (conf->show_help || conf->show_version))
    {
        show_options(conf);
        exit = 0;
    }

    printf("Hostname: ( %s )\n", conf->hostname);
    free(conf);
    return (exit);
}