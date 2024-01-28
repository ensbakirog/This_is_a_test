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
#include <stdlib.h>

int	take_fork(t_philo *philo)
{
	pthread_mutex_lock(&philo->l_fork);
	pthread_mutex_lock(philo->death);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(philo->death), 1);
	print_time("is taking fork", philo->index, philo->vars);
	if (philo->vars->is_dead == 1 || philo->vars->count == 1) //1 filozof varken ölüyor ancak programdan 
		return (pthread_mutex_unlock(philo->death),           //ölmeden önce çıktığı için ölü philo indexi 0 oluyor. Düzelt
			pthread_mutex_unlock(&philo->l_fork), 1);
	pthread_mutex_unlock(philo->death);
	pthread_mutex_lock(philo->r_fork);
	pthread_mutex_lock(philo->death);
	if (philo->vars->is_dead == 1 || philo->vars->count == 1)
		return (pthread_mutex_unlock(philo->death),
			pthread_mutex_unlock(&philo->l_fork), 1);
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
		
		if (vars->is_dead == 1)
			return (NULL);
		if (take_fork(philo) || eat(philo))
			return (NULL);
	}
	return (NULL);
}


void	die(t_vars *vars, int index)
{
	vars->is_dead = 1;
	print_time("\033[0;31mis dead", index, vars);
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
				if (vars->philos[i].eat_count >= vars->max_eat)
				{
					pthread_mutex_lock(vars->philos[i].death);
					vars->is_dead = 1;
					return (pthread_mutex_unlock(vars->philos[i].death), NULL);
				}
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
	threads_id = malloc(sizeof(pthread_t) * (vars.count -1));
	i = -1;
	while (++i < vars.count)
		pthread_create(&threads_id[i], NULL, eat_sleep_repeat, &vars.philos[i]);
	pthread_create(&azrael, NULL, death_note, &vars);
	i = -1;
	while (++i < vars.count)
		pthread_join(threads_id[i], NULL);
	pthread_join(azrael, NULL);
}
