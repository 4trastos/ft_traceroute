#define _POSIX_C_SOURCE 200809L

#include "ft_traceroute.h"

void    init_signal(void)
{
    struct  sigaction sa_int = {0};

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    
    sigaction(SIGINT, &sa_int, NULL);
}

void    sigint_handler(int signum)
{
    (void)signum;
    g_sigint_received = 1;
}