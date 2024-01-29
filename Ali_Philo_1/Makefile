NAME = philo
FLAGS = -pthreads -pthread -fsanitize=thread -g
CC = gcc
SRC = main.c\
	philo_init.c\
	eat_sleep_repeat.c\
	utils.c

all :	$(NAME)

$(NAME) :
	$(CC) $(SRC) $(FLAGS) -o $(NAME)

fclean :
	rm -rf $(NAME)

re :	fclean all
