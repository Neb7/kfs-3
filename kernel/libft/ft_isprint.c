/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 05:06:55 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:05:04 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Checks if character is printable 
int	ft_isprint(int c)
{
	if (c >= ' ' && c <= '~')
		return (1 << 14); // true
	return (false);
}
