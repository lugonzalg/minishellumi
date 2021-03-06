/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikgarci <mikgarci@student.42urduli>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/08 19:58:59 by mikgarci          #+#    #+#             */
/*   Updated: 2021/12/06 20:08:28 by mikgarci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "inc/minishell.h"
#include "inc/libft.h"
#include "inc/ft_printf.h"
#include "inc/get_next_line.h"
#include <dirent.h>

extern void	showenv(t_prompt *p)
{
	char	*line;
	int		fd;

	fd = open(p->envpath, O_RDONLY);
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		printf("%s", line);
		free(line);
	}
	close(fd);
}

int		check_env(t_child *child, char *line, t_prompt *p)
{
	char		*str;
	int			a;
	static int	is;

	if (!line && !is)
	{
		p->sizenv++;
		return (0);
	}
	if (!line && is)
	{
		is = 0;
		return (1);
	}
	str = ft_strchr(line, '=');
	a = ft_strncmp(child->info[1], line, str - line + 1);
	if (!a)
		is = 1;
	return (a);
}

void	deletenv_2(t_prompt *p)
{
	int		fd[2];
	char	*line;

	fd[0] = open(p->envpath, O_WRONLY | O_TRUNC);
	fd[1] = open(".envtemp", O_RDONLY);
	while (1)
	{
		line = get_next_line(fd[1]);
		if (!line)
			break ;
		write(fd[0], line, ft_strlen(line));
		free(line);
	}
	close(fd[0]);
	close(fd[1]);
	unlink(".envtemp");
}

extern void	envinclude(t_child	*child, t_prompt *p)
{
	int		fd[2];
	char	*line;

	(void) child;
	fd[0] = open(".envtemp", O_WRONLY | O_CREAT, 0644);
	fd[1] = open(p->envpath, O_RDONLY);
	while (1)
	{
		line = get_next_line(fd[1]);
		if (!check_env(child, line, p))
		{
			write(fd[0], child->info[1], ft_strlen(child->info[1]));
			write(fd[0], "\n", 1);
		}
		else
			write(fd[0], line, ft_strlen(line));
		if (!line)
			break ;
		free(line);
	}
	close(fd[0]);
	close(fd[1]);
	deletenv_2(p);
}

extern void	deletenv(t_child *child, t_prompt *p)
{
	int		fd[2];
	char	*line;

	fd[0] = open(p->envpath, O_RDONLY);
	fd[1] = open(".envtemp", O_WRONLY | O_CREAT, 0644);
	while (1)
	{
		line = get_next_line(fd[0]);
		if (!line)
			break ;
		if (ft_strncmp(child->info[1], line, ft_strlen(child->info[1])))
			write(fd[1], line, ft_strlen(line));
		else
			p->sizenv--;
		free(line);
	}
	close(fd[0]);
	close(fd[1]);
	deletenv_2(p);
}

extern void	ft_echo(t_child *child)
{
	size_t	i;
	bool	nl;
	int		fd;

	fd = 1;
	nl = true;
	i = 0;
	if (child->redir[1] || child->id < child->size[0] - 2)
		fd = child->fdpipe[child->id + 1][1];
	if (child->info[1] && !ft_strncmp(child->info[1], "-n", 3))
	{
		nl = false;
		i++;
	}
	while (child->info[++i])
	{
		write(fd, child->info[i], ft_strlen(child->info[i]));
		if (i < child->size[1] - 1)
			write(fd, " ", 1);
	}
	if (nl)
		write(fd, "\n", 1);
}
