#include "ft_traceroute.h"

int ft_strcmp(const char *s1, const char *s2)
{
    int i = 0;

    while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
        i++;
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int ft_atoi(char *str)
{
    int i = 0;
    int neg = 1;
    int result = 0;

    if (!str)
        return (0);
    
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || 
        str[i] == '\r' || str[i] == '\f' || str[i] == '\v')
        i++;
    
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            neg = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return (result * neg);
}

int ft_isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return (1);
    else
        return (0);
}

int is_valid_number(char *str)
{
    int i;

    i = 0;
    if (!str || *str == '\0')
        return (0);
    while (str[i] != '\0')
    {
        if (!ft_isdigit(str[i]))
            return (0);
        i++;
    }
    return (1);
}

int is_valid_interface_name(char *name)
{
    int i = 0;

    if (!name || *name == '\0')
        return (0);
    if (is_valid_number(name))
        return (0);
    while (name[i])
    {
        if (!ft_isalnum(name[i] || name[i] == '.' || name[i] == '_' || name[i] == '-'))
            return (0);
        i++;
    }
    return (1);
}

int ft_isalnum(int c)
{
    return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}