/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaltinto <aaltinto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 15:26:00 by aaltinto          #+#    #+#             */
/*   Updated: 2024/01/29 18:16:36 by aaltinto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>
#include <stdio.h>

int	take_fork(t_philo *philo)
{
	if (pthread_mutex_lock(&philo->l_fork) != 0)
		err_msg("cant forked");
	if (philo->vars->is_dead == 1)
		return (1);
	print_time("is taking fork", philo->index, philo->vars);
	if (philo->vars->is_dead == 1 || philo->vars->count == 1)
		return (pthread_mutex_unlock(&philo->l_fork), 1);
	pthread_mutex_lock(philo->r_fork);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->l_fork), pthread_mutex_unlock(philo->r_fork), 1);
	print_time("is taking fork", philo->index, philo->vars);	
	return (0);
}

int	eat(t_philo *philo)
{
	if (pthread_mutex_lock(philo->eat) != 0)
		err_msg("cant locked");
	print_time("\033[0;32mis eating", philo->index, philo->vars);
	ft_usleep(philo->vars->time_to_eat);
	philo->last_ate = get_time();
	philo->eat_count++;
	pthread_mutex_unlock(philo->eat);
	pthread_mutex_unlock(&philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
	if (pthread_mutex_lock(philo->death) != 0)
		err_msg("cant lock death");
	if (philo->vars->is_dead == 1)
		return (1);
	pthread_mutex_unlock(philo->death);
	if (pthread_mutex_lock(philo->sleep) != 0)
		err_msg("cant lock sleep");
	print_time("is sleeping", philo->index, philo->vars);
	ft_usleep(philo->vars->time_to_sleep);
	pthread_mutex_unlock(philo->sleep);
	pthread_mutex_lock(philo->death);
	if (philo->vars->is_dead == 1)
		return (1);
	pthread_mutex_unlock(philo->death);
	print_time("thinking", philo->index, philo->vars);
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
		if (vars->is_dead == 1)
			return (NULL);
		if (take_fork(philo) || eat(philo))
			return (NULL);
	}
	return (NULL);
}


void	die(t_vars *vars, int index)
{
	if (pthread_mutex_lock(vars->philos[index -1].death) != 0)
		perror("cant lock death in die");
	vars->is_dead = 1;
	print_time("\033[0;31mis dead", index, vars);
	pthread_mutex_unlock(vars->philos[index -1].death);
}

int	check_all(t_vars *vars)
{
	int	i;
	int	counter;
	
	i = -1;
	counter = 0;
	while (++i < vars->count)
		if (vars->philos[i].eat_count >= vars->max_eat)
			counter++;
	if (counter == vars->count)
	{
		vars->is_dead = 1;
		return (1);
	}
	return (0);
}

void	*death_note(void *arg)
{
	t_vars	*vars;
	int		i;

	vars = (t_vars *)arg;
	while (1)
	{
		i = -1;
		while (++i < vars->count)
		{
			if (vars->max_eat > -1)
			{
				pthread_mutex_lock(vars->philos[i].death);
				if (check_all(vars))
					return (pthread_mutex_unlock(vars->philos[i].death), NULL);
			}
			pthread_mutex_lock(vars->philos[i].death);
			if (get_time() - vars->philos[i].last_ate
				> vars->time_to_die)
				return (die(vars, vars->philos[i].index), pthread_mutex_unlock(vars->philos[i].death), NULL);
			pthread_mutex_unlock(vars->philos[i].death);
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
		pthread_mutex_destroy(vars->philos[i].death);
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
	philo_fill(argc, argv, &vars);
	threads_id = malloc(sizeof(pthread_t) * (vars.count -1));
	if (!threads_id)
		return (err_msg("malloc err"), abort_mission(&vars, &threads_id), EXIT_FAILURE);
	i = -1;
	while (++i < vars.count)
		pthread_create(&threads_id[i], NULL, eat_sleep_repeat, &vars.philos[i]);
	pthread_create(&azrael, NULL, death_note, &vars);
	i = -1;
	while (++i < vars.count)
		pthread_join(threads_id[i], NULL);
	pthread_join(azrael, NULL);
	return (abort_mission(&vars, &threads_id), EXIT_SUCCESS);
}
