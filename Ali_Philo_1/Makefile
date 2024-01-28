NAME = philo
FLAGS = -pthreads #-pthread -fsanitize=thread -g
CC = gcc
SRC = main.c\
	philo_init.c\
	utils.c

all :	$(NAME)

$(NAME) :
	$(CC) $(SRC) $(FLAGS) -o $(NAME)

fclean :
	rm -rf $(NAME)

re :	fclean all
