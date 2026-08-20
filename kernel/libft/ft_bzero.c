/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 03:18:05 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 18:50:51 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Erases data with \0 in n bytes starting from *s
void	ft_bzero(void *s, size_t n)
{
	if (!s)
		return ;
	ft_memset(s, '\0', n);
}
