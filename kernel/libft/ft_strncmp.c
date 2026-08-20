/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 16:25:11 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 19:07:30 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// This function compare s1 and s2 for n. Returns positive if value of S1 > S2, 
// negative otherwise and 0 if there is no difference. 
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int	cnt;

	if (!s1 || !s2)
		return (INT_MAX);
	cnt = 0;
	if (cnt == n)
		return (0);
	while ((s1[cnt] == s2[cnt]) && (cnt < n - 1) && (s1[cnt] && s2[cnt]))
		cnt++;
	return ((unsigned char) s1[cnt] - (unsigned char) s2[cnt]);
}
