/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 01:56:32 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:05:04 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Checks if character is digit (aka number from 0 to 9) 
int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1 << 11); // true
	return (false);
}
