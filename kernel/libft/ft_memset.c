/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 03:28:15 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/06/28 18:06:15 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Fills area of memory from *s with char c for n bytes. (bzero with extra step)
void	*ft_memset(void *s, int c, size_t n)
{
	size_t		i;
	long long	c8;

	i = -1;
	c8 = 0;
	while (++i < sizeof(long long))
		c8 = c8 | (long long)(unsigned char)c << sizeof(long long) * i;
	i = 0;
	while (n > sizeof(long long))
	{
		((long long *)s)[i] = c8;
		i++;
		n = n - sizeof(long long);
	}
	i = i * sizeof(long long);
	while (n > 0)
	{
		((unsigned char *)s)[i] = (unsigned char)c;
		i++;
		n--;
	}
	return (s);
}

/**
 * @brief	Fills area of memory from *s with short c for n shorts.
			Do not use wuith n odd size.
 * 
 * @param	s starting address of memory to fill
 * @param	c value to fill memory with
 * @param	n number of shorts to fill
 * @return void* 
 */
void	*ft_memset_short(void *s, int c, size_t n)
{
	size_t		i;
	long long	c8;

	i = 0;
	c8 = 0;
	while (i < sizeof(long long) / sizeof(unsigned short))
		c8 = c8 | (long long)(unsigned short)c << sizeof(long long) * i++ * sizeof(unsigned short);
	i = 0;
	while (n > sizeof(long long))
	{
		((long long *)s)[i] = c8;
		i++;
		n = n - sizeof(long long);
	}
	i = i * sizeof(long long) / sizeof(unsigned short);
	while (n > 0)
	{
		((unsigned short *)s)[i] = (unsigned short)c;
		i++;
		n = n - sizeof(unsigned short);
	}
	return (s);
}
