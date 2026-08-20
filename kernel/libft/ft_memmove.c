/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 06:47:04 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:53:20 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* Copies n bytes from memory area src to memory area dest.  
The memory areas may overlap: copying takes place 
as though the bytes in src are first copied 
from the end so there is no overlapping between src or dest.
*/
void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t	i;

	if (!dest || !src)
		return (NULL);
	i = n;
	if (dest > src && dest < src + n)
	{
		while (n % sizeof(long long) > 0)
		{
			((unsigned char *)dest)[i - 1] = ((unsigned char *)src)[i - 1];
			n--;
			i--;
		}
		i = i / sizeof(long long);
		while (n > 0)
		{
			((long long *)dest)[i - 1] = ((long long *)src)[i - 1];
			i--;
			n = n - sizeof(long long);
		}
	}
	else
		ft_memcpy(dest, src, n);
	return (dest);
}
