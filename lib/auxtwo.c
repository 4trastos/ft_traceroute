#include "ft_traceroute.h"

size_t  ft_strlen(char *str)
{
    size_t  len = 0;

    while (str[len] != '\0')
        len++;
    return (len);
}

void *ft_memcpy(void *dst, const void *src, size_t n)
{
    char *d;
    const char *s;
    size_t i;
	
	d = dst;
	s = src;
	i = 0;

    while (i < n) {
        d[i] = s[i];
        i++;
    }
    return (dst);
}