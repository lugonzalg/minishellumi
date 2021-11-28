/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unify.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lugonzal <lugonzal@student.42urduli>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/04 21:18:51 by lugonzal          #+#    #+#             */
/*   Updated: 2021/11/28 02:46:38 by lugonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "inc/get_next_line.h"
#include "inc/libft.h"
#include "inc/minishell.h"

extern void	cmd_size(t_child *child)
{
	int	pos;

	pos = 1;
	child->size[2] = child->size[1];
	while (child->info[pos])
	{
		if ((ft_strchr(child->info[pos - 1], INPUT)
				|| ft_strchr(child->info[pos - 1], OUTPUT))
			&& ft_strlen(child->info[pos - 1]) < 3)
		{
			*child->info[pos] = 0;
			*child->info[pos - 1] = 0;
			child->size[1] -= 2;
		}
		pos++;
	}
}

static void	resize_cat(t_child *child)
{
	char	**resize;
	size_t	i;

	i = 1;
	resize = ft_calloc(sizeof(char *), child->size[1] + 1);
	resize[0] = ft_strdup(child->info[0]);
	resize[1] = ft_strdup(".here_doc");
	i = 2;
	while (child->info[++i])
		resize[i] = ft_strdup(child->info[i]);
	free_d2(child->info);
	child->info = resize;
	child->size[2]--;
	child->redir[2] = true;
	child->redir[0] = true;
}

static void	here_doc(t_child *child, char *key)
{
	char	*line;
	int		fd;
	char	*key_nl;

	fd = open(".here_doc", O_RDWR | O_CREAT | O_TRUNC, 0644);
	close(child->fdpipe[child->id + 1][1]);
	child->fdpipe[child->id + 1][1] = fd;
	key_nl = ft_strjoin(key, "\n");
	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(0);
		if (!line)
			break ;
		if (!ft_strncmp(key_nl, line, ft_strlen(line)))
		{
			free(line);
			break ;
		}
		write(child->fdpipe[child->id + 1][1], line, ft_strlen(line));
		free(line);
	}
	free(key_nl);
	resize_cat(child);
}

extern void	unify_fdio(t_child *child)
{
	int		fd;
	size_t	i;

	i = -1;
	while (child->info[++i])
	{
		if (ft_strchr(child->info[i], OUTPUT))
		{
			if (ft_strlen(child->info[i]) == 1)
				fd = open(child->info[++i], O_RDWR | O_TRUNC | O_CREAT, 0644);
			else
				fd = open(child->info[++i], O_RDWR | O_TRUNC | O_CREAT, 0644);
			close(child->fdpipe[child->id + 1][1]);
			child->fdpipe[child->id + 1][1] = fd;
		}
		if (ft_strchr(child->info[i], INPUT))
		{
			if (ft_strlen(child->info[i]) == 1)
				fd = open(child->info[++i], O_RDONLY);
			else
				here_doc(child, child->info[i + 1]);
			close(child->fdpipe[child->id][0]);
			child->fdpipe[child->id][0] = fd;
		}
	}
}

void	unify_cmd(t_prompt *p, t_child *child)
{
	char	**temp;
	size_t	index;
	size_t	i;

	index = 0;
	i = 0;
	cmd_size(child);
	temp = (char **)ft_calloc(sizeof(char *), child->size[1] + 1);
	while (index < child->size[2] && child->info[index])
	{
		if (ft_strchr(child->info[index], '$') && ft_strchr(child->info[index], '\''))
				temp[i++] = expand_var(p, child, index);
		else if (child->info[index] && *child->info[index])
			temp[i++] = ft_strdup(child->info[index]);
		index++;
	}
	if (child->echo)
		temp = ft_realloc_child(temp);
	free_d2(child->info);
	child->info = temp;
}
