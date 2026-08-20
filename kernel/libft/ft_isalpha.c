/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 01:49:28 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:05:04 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Checks if character is from alphabet, either lower- or upper-case
int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1 << 10); // true
	return (false);
}
