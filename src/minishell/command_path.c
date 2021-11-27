/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lugonzal <lugonzal@student.42urduli>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/27 17:08:41 by lugonzal          #+#    #+#             */
/*   Updated: 2021/11/27 17:14:06 by lugonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/get_next_line.h"
#include "inc/minishell.h"
#include "inc/libft.h"
#include <stdlib.h>
#include <fcntl.h>

extern int	trim_path(t_child *child, int j)
{
	char	**tmp;
	int		i;

	tmp = ft_split(child->info[j], '/');
	i = 0;
	while (tmp[i])
		i++;
	free(child->info[j]);
	child->info[j] = ft_strdup(tmp[i - 1]);
	i = -1;
	free_d2(tmp);
	return (j);
}

static char	*ft_gnl_query(char *path)
{
	char	*line;
	int		fd;

	fd = open(path, O_RDONLY);
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			return (NULL);
		if (!ft_strncmp(line, "PATH=", 5))
			break ;
		free(line);
	}
	return (line);
}

extern char	**ft_setpath(t_prompt *p)
{
	char	**tmp;
	char	*line;
	int		i;

	line = ft_gnl_query(p->envpath);
	p->tmp = ft_strtrim(line, "PATH=\n");
	free(line);
	tmp = ft_split(p->tmp, ':');
	free(p->tmp);
	i = -1;
	while (tmp[++i])
	{
		line = ft_strjoin(tmp[i], "/");
		free(tmp[i]);
		tmp[i] = line;
	}
	return (tmp);
}