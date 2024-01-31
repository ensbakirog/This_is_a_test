/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaltinto <aaltinto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 15:26:00 by aaltinto          #+#    #+#             */
/*   Updated: 2024/01/31 12:35:55 by aaltinto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>
#include <stdio.h>

void	die(t_vars *vars, int index)
{
	if (pthread_mutex_lock(&vars->death) != 0)
		err_msg("Error\nMutex can't be locked");
	vars->is_dead = 1;
	print_time("\033[0;31mdied", index, vars);
	pthread_mutex_unlock(&vars->death);
}

int	check_all(t_vars *vars)
{
	int	i;
	int	counter;

	i = -1;
	counter = 0;
	pthread_mutex_lock(&vars->eat);
	while (++i < vars->count)
		if (vars->philos[i].eat_count >= vars->max_eat)
			counter++;
	if (counter == vars->count)
	{
		vars->is_dead = 1;
		return (pthread_mutex_unlock(&vars->eat), 1);
	}
	return (pthread_mutex_unlock(&vars->eat), 0);
}

void	*death_note(void *arg)
{
	t_vars	*vars;
	int		i;
	size_t	time;

	vars = (t_vars *)arg;
	while (1)
	{
		i = -1;
		while (++i < vars->count)
		{
			if (pthread_mutex_lock(&vars->death) != 0)
				return (err_msg("Error\nMutex can't be locked"), NULL);
			if (vars->max_eat > -1)
				if (check_all(vars))
					return (pthread_mutex_unlock(&vars->death), NULL);
			pthread_mutex_unlock(&vars->death);
			if (pthread_mutex_lock(&vars->eat) != 0)
				return (err_msg("Error\nMutex can't be locked"), NULL);
			time = get_time() - vars->philos[i].last_ate;
			pthread_mutex_unlock(&vars->eat);
			if (time > vars->time_to_die)
				return (die(vars, i +1), NULL);
		}
	}
}

void	abort_mission(t_vars *vars, pthread_t **threads_id)
{
	int	i;

	if (*threads_id != NULL)
		free(*threads_id);
	i = -1;
	while (++i < vars->count)
	{
		pthread_mutex_destroy(&vars->philos[i].l_fork);
		pthread_mutex_destroy(&vars->death);
		pthread_mutex_destroy(&vars->eat);
		pthread_mutex_destroy(&vars->sleep);
	}
	if (vars->philos != NULL)
		free(vars->philos);
}

int	main(int argc, char **argv)
{
	t_vars		vars;
	pthread_t	*threads_id;
	pthread_t	azrael;
	int			i;

	if (argc < 5 && argc > 6)
		return (err_msg(ARG_MSG), 1);
	if (!philo_fill(argc, argv, &vars))
		return (1);
	threads_id = malloc(sizeof(pthread_t) * (vars.count));
	if (!threads_id)
		return (err_msg("malloc err"), abort_mission(&vars, &threads_id), 1);
	i = -1;
	while (++i < vars.count)
		pthread_create(&threads_id[i], NULL, eat_sleep_repeat, &vars.philos[i]);
	pthread_create(&azrael, NULL, death_note, &vars);
	i = -1;
	while (++i < vars.count)
		pthread_join(threads_id[i], NULL);
	pthread_join(azrael, NULL);
	return (abort_mission(&vars, &threads_id), 0);
}
