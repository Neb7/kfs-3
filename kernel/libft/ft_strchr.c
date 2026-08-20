/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 07:17:51 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 14:56:51 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*Returns a pointer to the first occurrence of the character c in the string s.
*/
char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s != '\0')
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}
