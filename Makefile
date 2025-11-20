NAME = ft_traceroute
CC = gcc 
CFLAGS = -Wall -Wextra -Werror -g -Iincl
DLFLAGS = -lm
RM = rm -f

SRC = src/main.c src/parser.c src/signal_handler.c src/socket.c src/send_reply.c src/request.c \
	lib/aux.c src/show_options.c

OBTS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBTS) $(DLFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re