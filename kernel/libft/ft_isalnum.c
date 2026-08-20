/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 01:39:51 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:05:04 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Checks if character is digit (or number from 0 to 9) 
// or alphebetic, either upper- or lower-case.
int	ft_isalnum(int c)
{
	if (ft_isalpha(c) != false || ft_isdigit(c) != false)
		return (1 << 3); // true
	return (false);
}
