CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = src/server.c src/daemon.c src/handler.c src/utils.c
OBJ = $(SRC:.c=.o)

NAME = server

all: $(NAME)

$(NAME): $(OBJ)
	gcc $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
