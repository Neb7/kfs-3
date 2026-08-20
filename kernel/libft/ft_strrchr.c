/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 07:23:24 by vsyutkin          #+#    #+#             */
/*   Updated: 2023/10/29 18:56:53 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*Returns a pointer to the last occurrence of the character c in the string s.
*/
char	*ft_strrchr(const char *s, int c)
{
	char	*result;

	if (!s)
		return (NULL);
	result = NULL;
	while (*s != '\0')
	{
		if (*s == (char)c)
			result = ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (result);
}
