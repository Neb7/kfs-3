/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 06:05:42 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:54:32 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Search int c in memory area from *s for n bytes and return a pointer to it
void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*p;
	unsigned char		uc;
	size_t				i;

	if (!s)
		return (NULL);
	p = s;
	uc = c;
	i = 0;
	while (i < n)
	{
		if (p[i] == uc)
			return ((void *)(p + i));
		i++;
	}
	return (NULL);
}
