/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaltinto <aaltinto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 15:26:00 by aaltinto          #+#    #+#             */
/*   Updated: 2024/01/26 17:52:13 by aaltinto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>
#include <stdlib.h>

int	check_dead(t_vars *vars)
{
	pthread_mutex_lock(vars->death);
	if (vars->is_dead)
		return (pthread_mutex_unlock(vars->death), 1);
	pthread_mutex_unlock(vars->death);
	return (0);
}

int	take_fork(t_philo *philo)
{
	pthread_mutex_lock(&philo->l_fork);
	pthread_mutex_lock(philo->death);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(philo->death), 1);
	print_time("is taking fork", philo->index, philo->vars);
	pthread_mutex_unlock(philo->death);
	pthread_mutex_lock(philo->r_fork);
	pthread_mutex_lock(philo->death);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(philo->death), 1);
	print_time("is taking fork", philo->index, philo->vars);	
	return (0);
}

int	eat(t_philo *philo)
{
	print_time("\033[0;32mis eating", philo->index, philo->vars);
	pthread_mutex_unlock(philo->death);
	ft_usleep(philo->vars->time_to_eat);
	pthread_mutex_lock(philo->death);
	philo->last_ate = get_time();
	printf("eat %zu\n", (philo->last_ate));
	philo->eat_count++;
	pthread_mutex_unlock(philo->death);
	pthread_mutex_unlock(&philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(philo->death), 1);
	print_time("is sleeping", philo->index, philo->vars);
	ft_usleep(philo->vars->time_to_sleep);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(philo->death), 1);
	print_time("thinking", philo->index, philo->vars);
	pthread_mutex_unlock(philo->death);;
	return (0);
}

void	*eat_sleep_repeat(void *arg)
{
	t_philo	*philo;
	t_vars	*vars;

	philo = (t_philo *)arg;
	vars = philo->vars;
	if (philo->index % 2 == 0)
		ft_usleep(10);
	while (1)
	{
		if (philo->vars->max_eat > -1)
			if (philo->eat_count >= philo->vars->max_eat)
				return (NULL);
		if (vars->is_dead == 1)
			return (NULL);
		if (take_fork(philo) || eat(philo))
			return (NULL);
	}
	return (NULL);
}

static int	max_eat_fill(t_vars *vars, int argv, int i)
{
	if (argv <= 0)
		return (-1);
	vars->max_eat = argv;
	return (1);
}

int	philo_mutex_init(t_vars *vars)
{
	pthread_mutex_t	death;
	int				i;

	vars->philos = malloc(sizeof(t_philo) * vars->count);
	if (!vars->philos && pthread_mutex_init(&death, NULL) != 0)
		return (0);
	i = -1;
	while (++i < vars->count)
	{
		if (pthread_mutex_init(&vars->philos[i].l_fork, NULL) != 0)
			return (free(vars->philos), 0);
		if (i == vars->count -1)
			vars->philos[i].r_fork = &vars->philos[0].l_fork;
		else
			vars->philos[i].r_fork = &vars->philos[i + 1].l_fork;
		vars->philos[i].death = &death;
		vars->death = &death;
		vars->philos[i].eat_count = 0;
		vars->philos[i].index = i + 1;
		vars->philos[i].last_ate = get_time();
		vars->philos[i].vars = vars;
	}
	return (1);
}

int	philo_fill(int argc, char **argv, t_vars *vars)
{
	int	i;

	if (ft_atoi(argv[1]) <= 0 || ft_atoi(argv[2]) <= 0 || ft_atoi(argv[3]) <= 0
		|| ft_atoi(argv[4]) <= 0)
		return (err_msg("Invalid argument\nArguments must be\
			unsigned int"), 0);
	i = -1;
	while (++i < ft_atoi(argv[1]))
	{
		vars->count = ft_atoi(argv[1]);
		vars->time_to_die = ft_atoi(argv[2]);
		vars->time_to_eat = ft_atoi(argv[3]);
		vars->time_to_sleep = ft_atoi(argv[4]);
		vars->start_time = get_time();
		vars->is_dead = 0;
		if (argc == 6)
		{
			if (max_eat_fill(vars, ft_atoi(argv[5]), i) != 1)
				return (err_msg("Invalid argument\nArguments must\
					be positive"), 0);
		}
		else
			vars->max_eat = -1;
	}
	return (philo_mutex_init(vars));
}
#include <unistd.h>
void	die(t_vars *vars, int index)
{
	vars->is_dead = 1;
	write(1, "a\n", 2);
	print_time("is dead", index, vars);
}
#include <stdio.h>

void	*death_note(void *arg)
{
	t_vars	*vars;
	int		i;
	int		x;

	vars = (t_vars *)arg;
	x = 0;
	while (1)
	{
		i = -1;
		while (++i < vars->count)
		{
			if (x == 0)
			{
				ft_usleep(120);
				printf("death %zu\n", (get_time() - vars->philos[i].last_ate));
				x++;
			}
			pthread_mutex_lock(vars->philos[i].death);
			if (get_time() - vars->philos[i].last_ate
				> vars->time_to_die)
				return (die(vars, i), pthread_mutex_unlock(vars->philos[i].death), NULL);
			pthread_mutex_unlock(vars->philos[i].death);
		}
	}
}

int	main(int argc, char **argv)
{
	t_vars		vars;
	pthread_t	*threads_id;
	pthread_t	azrael;
	int			i;

	if (argc < 5 && argc > 6)
		return (err_msg(ARG_MSG), 1);
	philo_fill(argc, argv, &vars);
	threads_id = malloc(sizeof(pthread_t) * (vars.count - 1));
	i = -1;
	while (++i < vars.count)
		pthread_create(&threads_id[i], NULL, eat_sleep_repeat, &vars.philos[i]);
	pthread_create(&azrael, NULL, death_note, &vars);
	i = -1;
	while (++i < vars.count)
		pthread_join(threads_id[i], NULL);
	pthread_join(azrael, NULL);
}
