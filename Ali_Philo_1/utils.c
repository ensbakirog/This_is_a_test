/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaltinto <aaltinto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:57:56 by aaltinto          #+#    #+#             */
/*   Updated: 2024/01/26 16:15:55 by aaltinto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "philo.h"

int	ft_atoi(char *num)
{
	int	ret;
	int	i;

	ret = 0;
	i = -1;
	while (num[++i])
	{
		if (num[i] > '9' && num[i] > '0')
			return (0);
		ret = (ret * 10) + (num[i] - 48);
	}
	return (ret);
}

void	err_msg(char *msg)
{
	int	i;

	i = 0;
	while (msg[i])
		i++;
	write(2, msg, i);
	write(2, "\n", 1);
}

size_t	get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) != 0)
		err_msg("gettimeofday() error");
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void	print_time(char *msg, int i, t_vars *vars)
{
	size_t	p_time;

	p_time = get_time() - vars->start_time;
	printf("%ld  [%d]  %s\033[0;37m\n", p_time, i, msg);
}

int	ft_usleep(size_t milliseconds)
{
	size_t	start;

	start = get_time();
	while ((get_time() - start) < milliseconds)
		usleep(milliseconds / 10);
	return (0);
}
