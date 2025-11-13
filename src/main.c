#include "ft_traceroute.h"

volatile sig_atomic_t g_sigint_received = 0;

int main(int argc, char **argv)
{
    int exit = 0;

    if (argc < 2)
    {
        printf("%s: usage error: Destination address required \n", argv[0]);
        return (1);
    }

    return (0);
}