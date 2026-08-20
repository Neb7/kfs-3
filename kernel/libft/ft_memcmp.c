/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 06:22:14 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:54:10 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// This function compare s1 and s2 for n. Returns positive if value of S1 > S2, 
// negative otherwise and 0 if there is no difference. 
int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t	i;

	if (!s1 || !s2)
		return (0);
	i = 0;
	while (i < n)
	{
		if (*(unsigned char *)(s1 + i) != *(unsigned char *)(s2 + i))
			return (*(unsigned char *)(s1 + i) - *(unsigned char *)(s2 + i));
		i++;
	}
	return (0);
}
