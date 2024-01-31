/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat_sleep_repeat.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaltinto <aaltinto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 17:34:13 by aaltinto          #+#    #+#             */
/*   Updated: 2024/01/31 13:34:52 by aaltinto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	take_fork(t_philo *philo)
{
	if (pthread_mutex_lock(&philo->l_fork) != 0)
		return (err_msg("Error\nFork can't be locked"), 1);
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->vars->death),
			pthread_mutex_unlock(&philo->l_fork), 1);
	pthread_mutex_unlock(&philo->vars->death);
	print_time("has taken a fork", philo->index, philo->vars);
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1 || philo->vars->count == 1)
		return (pthread_mutex_unlock(&philo->vars->death),
			pthread_mutex_unlock(&philo->l_fork), 1);
	pthread_mutex_unlock(&philo->vars->death);
	if (pthread_mutex_lock(philo->r_fork) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->l_fork),
			pthread_mutex_unlock(philo->r_fork),
			pthread_mutex_unlock(&philo->vars->death), 1);
	pthread_mutex_unlock(&philo->vars->death);
	return (print_time("has taken a fork", philo->index, philo->vars), 0);
}

int	eat(t_philo *philo)
{
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->vars->death), 1);
	pthread_mutex_unlock(&philo->vars->death);
	print_time("\033[0;32mis eating", philo->index, philo->vars);
	ft_usleep(philo->vars->time_to_eat);
	if (pthread_mutex_lock(&philo->vars->eat) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	philo->last_ate = get_time();
	philo->eat_count++;
	pthread_mutex_unlock(&philo->vars->eat);
	pthread_mutex_unlock(&philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->vars->death), 1);
	pthread_mutex_unlock(&philo->vars->death);
	return (0);
}

int	sleep_think(t_philo *philo)
{
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->vars->death), 1);
	pthread_mutex_unlock(&philo->vars->death);
	print_time("is sleeping", philo->index, philo->vars);
	ft_usleep(philo->vars->time_to_sleep);
	if (pthread_mutex_lock(&philo->vars->death) != 0)
		return (err_msg("Error\nMutex can't be locked"), 1);
	if (philo->vars->is_dead == 1)
		return (pthread_mutex_unlock(&philo->vars->death), 1);
	pthread_mutex_unlock(&philo->vars->death);
	print_time("is thinking", philo->index, philo->vars);
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
		if (take_fork(philo))
			return (NULL);
		if (eat(philo))
			return (NULL);
		if (sleep_think(philo))
			return (NULL);
	}
	return (NULL);
}
