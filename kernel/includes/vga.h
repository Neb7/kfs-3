/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vga.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 15:07:56 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/28 16:37:06 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VGA_H
# define VGA_H

# include "kernel.h"

typedef struct	s_vga
{
	uint16_t	cursor_x;
	uint16_t	cursor_y;
	uint16_t	lines[25][80];
	uint8_t		color;
}	t_vga;

extern t_vga	g_screens[2];
extern int		g_cur;

void	switch_screen(int screen);
void	scroll();

#endif
