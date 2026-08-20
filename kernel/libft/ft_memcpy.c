/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 06:31:15 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:53:25 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* The  memcpy() function copies n bytes 
from memory area src to memory area dest. 
The memory areas must not overlap. 
*/
void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;

	i = 0;
	if (dest != NULL && src != NULL)
	{
		while (n >= sizeof(long long))
		{
			((long long *)dest)[i] = ((long long *)src)[i];
			i++;
			n = n - sizeof(long long);
		}
		i = i * sizeof(long long);
		while (n > 0)
		{
			((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
			n--;
			i++;
		}
	}
	return (dest);
}
