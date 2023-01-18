# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

// Token types
// builtin : builtin cmd: e.g. echo, cd
// cmd: any type of other command
// arg : any type of argument
// input : '<' 
// heredoc : '<<'
// trunc : '>'
// append : '>>'
// pipe : '|'

# define BUILTIN 1
# define CMD 2
# define ARG 3
# define INPUT 4
# define HEREDOC 5
# define TRUNC 6
# define APPEND 7
# define PIPE 8
# define PATH 9
# define CAT 10

// str - token string
// type - token type
// prev and next - linked list pointers to previous and next token

typedef struct s_token
{
	char			*str;
	int				type;
	struct s_token	*prev;
	struct s_token	*next;
}	t_token;

typedef struct s_mini
{
	t_token		*tokens;
	int			cmd;
}	t_mini;

int	ft_strcmp(char *s1, char *s2)
{
	while (*s1 && *s2 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

int path_cmd(char *token)
{
	if (!ft_strcmp(token, "ls")
		|| !ft_strcmp(token, "grep")
		|| !ft_strcmp(token, "wc"))
		return (9);
	return (0);
}

int path_cat(char *token)
{
	if (!ft_strcmp(token, "cat"))
		return (10);
	return (0);

}


//get builtin command
int	builtin_cmd(char *token)
{
	if (!ft_strcmp(token, "echo")
		|| !ft_strcmp(token, "cd")
		|| !ft_strcmp(token, "pwd")
		|| !ft_strcmp(token, "export")
		|| !ft_strcmp(token, "unset")
		|| !ft_strcmp(token, "env")
		|| !ft_strcmp(token, "exit")
		|| !ft_strcmp(token, "history"))
		return (1);
	return (0);
}

//to detect delimiter tokens
int	delim_token(char *token)
{
	if (!ft_strcmp(token, ">"))
		return (TRUNC);
	if (!ft_strcmp(token, ">>"))
		return (APPEND);
	if (!ft_strcmp(token, "<"))
		return (INPUT);
	if (!ft_strcmp(token, "<<"))
		return (HEREDOC);
	if (!ft_strcmp(token, "|"))
		return (PIPE);
	return (0);
}

int do_path(char *token)
{

	int fd;
	int fds[2];
	int res;
	
	char cmd[] = "/bin/ls";
	char *arg[] = {"ls", NULL};
	char *env[] = {NULL};

	int pip = (*cmd);
	int pid = fork();
	
	if (!pid)
	{
		printf("%d\n", pid);
		execve(cmd, arg, env);
		kill(0, SIGKILL);
		
	}
	if (waitpid(pid, &res, 0) == -1)
	printf("error");
	return WIFEXITED(res) && WEXITSTATUS(res);

}

int built_incat(char *token)
{
	int fd;
	int fds[2];
	int res;

	char cmd[] = "/bin/cat";
	char *arg[] = {"/bin/cat","/Users/abasarud/desktop/text.txt", NULL};
	char *env[] = {NULL};

	int pip = (*cmd);
	int pid = fork();

	if (!pid)
	{
		printf("%d\n", pid);
		execve(cmd, arg, env);
		kill(0, SIGKILL);
		
	}
	if (waitpid(pid, &res, 0) == -1)
	printf("error");
	return WIFEXITED(res) && WEXITSTATUS(res);
	

}

//to get the token types from input
int	token_type(t_mini *mini, char *token)
{
	if (delim_token(token))
	{
		mini->cmd = 1;
		return (delim_token(token));
	}
	if (!mini->cmd)
		return (ARG);
	else
	{
		mini->cmd = 0;
		if (builtin_cmd(token))
			return (BUILTIN);
		if (path_cmd(token))
		{
			do_path(token);
			return (PATH);
		}
		if (path_cat(token))
		{
			built_incat(token);
			return (CAT);
		}

	}
	return 0;
}

char	*ft_strdup(const char *s1)
{
	char	*dup;
	int		i;
	int		len;

	i = 0;
	len = 0;
	while (s1[len])
		len++;
	dup = (char *)malloc(sizeof(char) * (len + 1));
	if (!dup)
		return (NULL);
	while (i < len)
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

//create new token list
t_token	*new_token(t_mini *mini, char *data)
{
	t_token	*res;

	res = (t_token *)malloc(sizeof(t_token));
	res->str = ft_strdup(data);
	res->type = token_type(mini, data);
	res->prev = 0;
	res->next = 0;
	return (res);
}

//addes a token to the end of the list
void	token_addend(char *data, t_mini *mini)
{
	t_token	*tok;
	t_token	*temp;

	tok = mini->tokens;
	while (tok->next)
		tok = tok->next;
	temp = new_token(mini, data);
	tok->next = temp;
	tok->next->prev = tok;
}

//prints out the different tokens and their types
void print_tokens(t_token *tokens)
{
 	t_token	*curr;

 	curr = tokens;
 	while (curr)
 	{
 		printf("token : |%s| type : %d\n", curr->str, curr->type);
 		curr = curr->next;
 	}
}

static int	ft_wordcounter(char const *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		i++;
	}
	return (count);
}

static char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;
	size_t	i;

	i = 0;
	dup = (char *)malloc(sizeof(char) * n + 1);
	if (!dup)
		return (NULL);
	while (s[i] && i < n)
	{
		dup[i] = s[i];
		i++;
	}
	dup[n] = '\0';
	return (dup);
}

char	**ft_split(char const *s, char c)
{
	char	**result;
	size_t	i;
	size_t	j;
	size_t	len;

	i = 0;
	j = 0;
	if (!s)
		return (NULL);
	result = (char **)malloc(sizeof(char *) * (ft_wordcounter(s, c) + 1));
	if (!result)
		return (NULL);
	while (s[i])
	{
		while (s[i] == c)
			i++;
		len = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > len)
			result[j++] = ft_strndup(&s[len], i - len);
	}
	result[j] = NULL;
	return (result);
}

void	parse(t_mini *mini, char *buff)
{
	char	**split;
	int		i;
	t_token	*head;

	split = ft_split(buff, ' ');
	head = new_token(mini, *split);
	mini->tokens = head;
	i = 0;
	while (split[++i])
		token_addend(split[i], mini);
	print_tokens(mini->tokens);
	mini->cmd = 1;
}

t_mini	*init_mini(void)
{
	t_mini	*mini;

	mini = (t_mini *)malloc(sizeof(t_mini));
	mini->tokens = 0;
	mini->cmd = 1;
	return (mini);
}

//just to get rid of unused parameter error when compiling
void	init_main(int argc, char **argv)
{
	if (argc > 1 || argv[1])
	{
		printf("one argc ./minishell ");
		exit(1);
	}
}

int	main(int argc, char *argv[])
{
	t_mini	*mini;
	char	*buff;

	init_main(argc, argv);
	mini = init_mini();
	while (1)
	{
		buff = readline("@minishell> ");
		parse(mini, buff);
	}
	return (0);
}