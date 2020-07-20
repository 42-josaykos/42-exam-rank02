/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonny <josaykos@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/08 20:20:20 by jonny             #+#    #+#             */
/*   Updated: 2020/07/12 18:02:19 by jonny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct	s_specs
{
	char	type;
	int		width;
	int		precision;
}				t_specs;

static void	ft_putchar(char c, int *ret)
{
	write(1, &c, 1);
	*ret += 1;
}

static void	ft_putstr(char *str, int *ret)
{
	int i;

	i = 0;
	while (str[i])
		ft_putchar(str[i++], ret);
}

static int			mini_atoi(const char *str, int *i)
{
	unsigned int nb;

	nb = 0;
	while (str[*i] >= '0' && str[*i] <= '9')
	{
		nb = nb * 10 + (str[*i] - 48);
		(*i)++;
	}
	return (nb);
}

static void	check_specs(const char *str, int *i, t_specs *specs)
{
	specs->type = -1;
	specs->width = -1;
	specs->precision = -1;
	if (str[*i] >= '0' && str[*i] <= '9')
		specs->width = mini_atoi(str, i);
	if (str[*i] == '.')
	{
		++(*i);
		if (str[*i] >= '0' && str[*i] <= '9')
				specs->precision = mini_atoi(str, i);
		else
			specs->precision = 0;
	}
	if (str[*i] == 's' || str[*i] == 'd' || str[*i] == 'x')
	{
		specs->type = str[*i];
		(*i)++;
	}
}

static int	strlen_base(long int nb, int base)
{
	int i;

	i = 0;
	if (nb < 0)
		i++;
	while (nb != 0 && i++ < base)
		nb = nb / base;
	return (i);
}

static char	*print_zero_base(void)
{
	char *str;

	if (!(str = malloc(sizeof(char) * 2)))
		return (NULL);
	str[0] = '0';
	str[1] = '\0';
	return (str);
}

static char		*itoa_base(long int nb, int base, char *tab)
{
	char		*str;
	int			len;
	long int	tmp;

	len = strlen_base(nb, base);
	tmp = nb;
	if (nb == 0)
		return (print_zero_base());
	else
	{
		if (nb < 0)
			tmp *= -1;
		if (!(str = malloc(sizeof(char) * len)))
			return (NULL);
		str[len] = '\0';
		if (nb < 0)
			str[0] = '-';
		while (len > 0 && tmp != 0)
		{
			str[--len] = tab[tmp % base];
			tmp = tmp / base;
		}
	}
	return (str);
}

static void	nbr_zero_padding(t_specs *specs, char *str, int len, int *ret)
{
	int precision;

	precision = specs->precision;
	if (str[0] == '-')
	{
		ft_putchar('-', ret);
		str++;
		precision++;
	}
	while (specs->width > len || precision > len)
	{
		ft_putchar('0', ret);
		len++;
	}
	if (!(precision == 0 && str[0] == '0'))
		ft_putstr(str, ret);
}

static void	nbr_whitespace(t_specs *specs, char *str, int len, int *ret)
{
	if (str[0] == '-' && specs->precision >= len)
		specs->width--;
	while (specs->width > len && specs->width > specs->precision
			&& specs->precision != 0)
	{
		ft_putchar(' ', ret);
		specs->width--;
	}
	if (specs->width > 0 && specs->precision == 0 && str[0] == '0')
		specs->width++;
	while (specs->width > len && specs->precision <= 0)
	{
		ft_putchar(' ', ret);
		specs->width--;
	}
}

static void		check_nbr(t_specs *specs, char *str, int *ret)
{
	int len;

	len = 0;
	while (str[len] != '\0')
		len++;
	if ((specs->width == -1 && specs->precision > len) ||
			specs->width < specs->precision)
		nbr_zero_padding(specs, str, len, ret);
	else if (specs->width >= 0)
	{
		if (specs->width > len && specs->precision == -1)
		{
			nbr_whitespace(specs, str, len, ret);
			ft_putstr(str, ret);
		}
		else if (specs->width >= specs->precision)
		{
	  		nbr_whitespace(specs, str, len, ret);
	  		nbr_zero_padding(specs, str, len, ret);
		}
	}
	else
		ft_putstr(str, ret);
	free(str);
	str = NULL;
}

static void	print_str(va_list params, t_specs *specs, int *ret)
{
	char	*str;
	int		len;
	int i;

	len = 0;
	str = va_arg(params, char*);
	i = 0;
	if (!str)
		str = "(null)";
	while ((len < specs->precision || specs->precision == -1) && str[len])
		len++;
	while (specs->width - len > 0)
	{
		ft_putchar(' ', ret);
		specs->width--;
	}
	while (i < len)
		ft_putchar(str[i++], ret);
}

static void	print_integer(va_list params, t_specs *specs, int *ret)
{
	int				nb;
	char			*str;

	nb = va_arg(params, int);
	str = itoa_base(nb, 10, "0123456789");
	check_nbr(specs, str, ret);
}

static void	print_hex(va_list params, t_specs *specs, int *ret)
{
	char			*tab;
	char			*str;
	unsigned int	nb;

	tab = "0123456789abcdef";
	nb = va_arg(params, unsigned int);
	str = itoa_base(nb, 16, tab);
	check_nbr(specs, str, ret);
}

static void	print_type(va_list params, t_specs *specs, int *ret)
{
	if (specs->type == 's')
		print_str(params, specs, ret);
	else if (specs->type == 'd')
		print_integer(params, specs, ret);
	else if (specs->type == 'x')
		print_hex(params, specs, ret);
}

static int	parse_str(va_list params, const char *str, t_specs *specs, int *ret)
{
	int		i;

	i = 0;
	while (str[i] != '\0')
	{
		while (str[i] != '%' && str[i] != '\0')
			ft_putchar(str[i++], ret);
		if (str[i] == '%')
		{
			i++;
			check_specs(str, &i, specs);
			if (specs->type == 's' || specs->type == 'd' || specs->type == 'x')
				print_type(params, specs, ret);
			else
				break ;
		}
	}
	return (*ret);
}

int			ft_printf(const char *str, ...)
{
	int		ret;
	va_list	params;
	t_specs	*specs;

	ret = 0;
	va_start(params, str);
	if (!(specs = malloc(sizeof(t_specs))))
		return (-1);
	parse_str(params, str, specs, &ret);
	va_end(params);
	free(specs);
	return (ret);
}
