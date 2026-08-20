/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 05:48:48 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:07:09 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Welp, ft_strlcat but consider dest as empty. Version: Clean
size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	dest_len;
	size_t	src_len;
	size_t	i;

	if (!dst || !src)
		return (INT_MAX);
	dest_len = 0;
	src_len = 0;
	i = 0;
	while (src[src_len])
		src_len++;
	if (dest_len >= size)
		return (size + src_len);
	while (src[i] && dest_len + i < size - 1)
	{
		dst[dest_len + i] = src[i];
		i++;
	}
	dst[dest_len + i] = '\0';
	return (dest_len + src_len);
}
