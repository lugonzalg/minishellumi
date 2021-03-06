/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lugonzal <lugonzal@student.42urduli>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/03 13:37:46 by lugonzal          #+#    #+#             */
/*   Updated: 2021/12/06 21:12:04 by mikgarci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "inc/minishell.h"
#include "inc/libft.h"
#include "inc/ft_printf.h"
#include "inc/get_next_line.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <term.h>

void	check_redir(t_prompt *p, t_child *child)
{
	if (ft_strchr(p->d2_prompt[child->id], INPUT))
		child->redir[0] = true;
	if (ft_strchr(p->d2_prompt[child->id], OUTPUT))
		child->redir[1] = true;
	child->info = ft_split_ptr(p->d2_prompt[child->id],
			' ', ft_len_redir);
	ft_expand(p, child);
	while (child->info[child->size[1]])
		child->size[1]++;
	if (child->redir[0] || child->redir[1])
		unify_fdio(child);
	unify_cmd(p, child);
	if (!child->builtin || !ft_strncmp(child->info[0], "expr", 4))
		command_pos(p, child);
}

int	go_exit(int n)
{
	g_glob.error = n;
	return (n);
}

void	multipipe(t_child *child)
{
	size_t	i;
	ssize_t	signal;

	i = 0;
	while (i < child->size[0])
	{
		if (i != child->id)
			close(child->fdpipe[i][0]);
		if (i != child->id + 1)
			close(child->fdpipe[i][1]);
		i++;
	}
	if (child->id || child->redir[0])
	{
		dup2(child->fdpipe[child->id][0], 0);
		close(child->fdpipe[child->id][0]);
	}
	if (child->id < child->size[0] - 2 || child->redir[1])
	{
		dup2(child->fdpipe[child->id + 1][1], 1);
		close(child->fdpipe[child->id + 1][1]);
	}
	signal = execve(child->path, child->info, NULL);
	exit(127);
}

static void	restart_data(t_child *child)
{
	ft_memset(&child->size[1], 0, sizeof(size_t) * 3);
	ft_memset(child->redir, false, sizeof(bool) * 2);
	free_d2(child->info);
	free(child->path);
}

static void	process_command(t_prompt *p, t_child *child, size_t i)
{
	child->id = i;
	check_redir(p, child);
	if (ft_checkbuiltins(child->info[0], p))
		ft_builtins(child, p);
	else if (child->info[0])
	{
		p->id[i] = fork();
		g_glob.killid = p->id[1];
		if (p->id[i] == 0)
			multipipe(child);
		else
		{
			if (access(child->path, X_OK) || !ft_strncmp(child->info[i], getenv("PWD"), ft_strlen(child->info[i])) || !ft_strncmp(child->info[i], getenv("HOME"), ft_strlen(child->info[i])))
			{
				printf("minishell: %s: is a directory\n", child->info[i]);
				go_exit(127);
			}
		}
	}
	restart_data(child);
}

static void	process_io(t_prompt *p)
{
	size_t	i;
	t_child	child;
	int		status;

	status = 0;
	set_child(p, &child);
	i = -1;
	while (p->d2_prompt[++i])
		process_command(p, &child, i);
	free_child(&child);
	while (1)
	{
		if (wait(&status) <= 0)
			break ;
	}
}

static bool	check_prompt(t_prompt *p)
{
	size_t	i;

	p->d2_prompt = ft_split_ptr(p->prompt, '|', ft_lenp);
	i = -1;
	add_history(p->prompt);
	while (p->d2_prompt[++i])
	{
		if (!ft_quote_error(p->d2_prompt[i]))
		{
			free_d2(p->d2_prompt);
			free(p->prompt);
			return (true);
		}
	}
	process_io(p);
	free(p->id);
	free(p->prompt);
	return (false);
}

extern void	prompt_io(t_prompt *p)
{
	while (1)
	{
		//rl_catch_signals = 0;
		g_glob.killid = 0;
		p->prompt = readline("minishell > ");
		if (!p->prompt || !ft_strncmp(p->prompt, "exit", sizeof("exit")))
			break ;
		rl_on_new_line();
		if (check_prompt(p))
			continue ;
		free_d2(p->d2_prompt);
	}
	//rl_clear_history();
}
