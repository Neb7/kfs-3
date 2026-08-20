/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 07:30:58 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:12:33 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* Applies function 'f' for each character of string *s, 
passing it's index as first argument of the function*/
void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	int	index;

	index = 0;
	while (*s && f && s)
	{
		f(index, s);
		s++;
		index++;
	}
}
