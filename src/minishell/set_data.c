/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_data.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lugonzal <lugonzal@student.42urduli>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/03 13:38:35 by lugonzal          #+#    #+#             */
/*   Updated: 2021/11/05 19:47:33 by lugonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "inc/minishell.h"
#include "inc/libft.h"
#include <fcntl.h>

extern void	set_str(t_string *str)
{
	int	i;

	ft_memset(str, 0, sizeof(t_string));
	str->user = ft_strjoin(getenv("USER"), " \e[1;37mminishell \e[0;m% ");
	str->path = ft_split(getenv("PATH"), ':');
	i = -1;
	while (str->path[++i])
	{
		str->tmp = ft_strjoin(str->path[i], "/");
		free(str->path[i]);
		str->path[i] = str->tmp;
	}
	str->tmp = NULL;
}

extern void	set_child(t_string *str, t_child *child)
{
	size_t	i;
	ssize_t	id;
		
	i = 1;
	while (str->d2_prompt[i - 1])
		i++;
	ft_memset(child, 0, sizeof(t_child));
	child->fdpipe = (int **)malloc(sizeof(int *) * i);
	child->size[0] = i;
	while (i--)
	{
		child->fdpipe[i] = (int *)malloc(sizeof(int) * 2);
		pipe(child->fdpipe[i]);
	}
	id = ttyslot();
	//close(child->fdpipe[child->size[0] - 1][1]);
	//child->fdpipe[child->size[0] - 1][1] = 2;
	
}
